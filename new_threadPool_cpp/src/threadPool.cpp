#include "threadPool.h"
#include "Task.h"
#include "TaskQueue.h"
#include "TaskQueue.cpp"    //需要包含源文件，否则模板类 的函数就会 报错 “未定义的引用”
#include <iostream>
#include <string.h>
#include <string>
// #include <pthread.h>
#include <thread>
#include <unistd.h>

using namespace std;

template <typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
    //可以使用break 替代 return，在函数内部退出
    do
    {
        //实例化任务队列
        taskQ=new TaskQueue<int>;
        if(taskQ==nullptr)
        {
            cout<<"malloc taskQ fail...\n";
            break;
        }

        //初始化结构体成员
        threadIDs= new pthread_t[max];
        if(threadIDs==nullptr)
        {
            //内存分配失败
            cout<<"malloc pthreadIDs fail...\n";
            break;
        }
        
        //初始化线程
        memset(threadIDs,0,sizeof(pthread_t)*max);
        minNum=min;
        maxNum=max;
        busyNum=0;
        liveNum=min;
        exitNum=0;

        //初始化线程
        if(pthread_mutex_init(&mutexPool,NULL)!=0 ||
            pthread_cond_init(&notEmpty,NULL)!=0)
        {
            //初始化线程失败
            cout<<"mutex or condition init fail...\n";

            //return NULL;
            break;
        }

        //初始时不销毁
        shutDown=false;

        //创建线程
        //这里 manager需要是非成员函数，或者作为 静态成员，不再属于对象，而是属于类，就有了地址，就可以传递函数地址了
        pthread_create(&managerID,NULL,manager,this); //管理者线程, 使用this传递静态实例对象，manager就可以管理非静态成员了
        for(int i=0;i<min;i++)
        {
            pthread_create(&threadIDs[i],NULL,worker,this);   //工作者线程
        }

        //申请空间成功，返回申请的线程池
        return ;

    } while (0);    //如果出现异常，就会推出此循环，进行下面资源的释放
    
    //资源释放
    if(threadIDs)
    {
        delete[] threadIDs;
    }
    if(taskQ)
    {
        delete taskQ;
    }

}

//工作函数
template <typename T>
void * ThreadPool<T>::worker(void* arg)
{
    //类型转换
    ThreadPool* pool=static_cast<ThreadPool*>(arg);
    
    while(true)
    {
        //线程使用之前加锁
        pthread_mutex_lock(&pool->mutexPool);

        //判断当前任务队列
        while (pool->taskQ->taskNumber()==0&& !pool->shutDown)
        {
            //阻塞工作线程
            pthread_cond_wait(&pool->notEmpty,&pool->mutexPool);

            //销毁 阻塞的工作线程
            if(pool->exitNum>0)
            {
                pool->exitNum--;
                if (pool->liveNum>pool->minNum)
                {
                    //减少线程数
                    pool->liveNum--;
                    //解开互斥锁
                    pthread_mutex_unlock(&pool->mutexPool);

                    //销毁线程
                    pool->threadExit();
                }
     
            }
        }

        //判断线程池是否被关闭了
        if(pool->shutDown)
        {
            //先解锁, 避免死锁
            pthread_mutex_unlock(&pool->mutexPool);
            //退出线程
            pool->threadExit();
        }
        
        //从任务队列中取出一个任务
        Task<T> task=pool->taskQ->takeTask();

        //解锁
        pool->busyNum++;
        //线程使用完之后解锁
        pthread_mutex_unlock(&pool->mutexPool);
        

        cout<<"thread "<<to_string(pthread_self())<<" start working...\n";

        //取出任务, 处理任务
        task.function(task.arg);

        //任务处理结束后，要释放掉 堆内存, 即传入的num
        delete task.arg;    //这里arg为void* 如果该内存大于4个字节，那么这里删除指针，就有可能会无法完全释放
        task.arg=nullptr;   //那么这里为了方便释放内存，可以将其变为 类模板

        cout<<"thread "<<pthread_self()<<" end working...\n";

        //任务处理结束后，需要将 忙线程数再减少
        pthread_mutex_lock(&pool->mutexPool); //线程会多访问 加锁
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);   //解锁
    }
    return NULL;
}

//管理者线程
template <typename T>
void * ThreadPool<T>::manager(void* arg)
{
    //强制类型转换
    ThreadPool* pool=static_cast<ThreadPool*>(arg);
    //按照一定的频率，检测 后 调整线程个数
    while(!pool->shutDown)  //线程池未关闭就检测
    {
        //每隔3S 检测一次
        sleep(3);

        //取出线程池中任务的数量和当前线程池的数量, 防止有其他线程在写入数据，所以需要 锁
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize=pool->taskQ->taskNumber();
        int liveNum=pool->liveNum;
        int busyNum=pool->busyNum;
        pthread_mutex_unlock(&pool->mutexPool);

        //添加线程
        //任务个数>存活的线程个数 并且 存活的线程个数<最大线程数 时，才会继续增加新线程
        if(queueSize>liveNum&&liveNum<pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter=0;
            //不仅要在上面判断，也需要在 这里再次判断，防止在这期间数量发送变化，导致 个数出现问题
            for(int i=0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum;i++)
            {
                //找到未使用的线程ID(被销毁的线程)
                if(pool->threadIDs[i]==0)
                {
                    //创建线程, 直接使用该线程ID
                    pthread_create(&pool->threadIDs[i],NULL,worker,pool);
                    counter++;  //总线程个数
                    pool->liveNum++;    //存活线程个数
                }
            }
            pthread_mutex_unlock(&pool->mutexPool); 
        }

        //销毁线程
        //销毁线程的条件：忙线程*2<存活线程 并且 存活的线程>最小线程数
        if(busyNum*2<liveNum && liveNum>pool->minNum)
        {
            //操作线程池中的值都要加锁
            pthread_mutex_lock(&pool->mutexPool);
            //每次销毁后值保留两个线程
            pool->exitNum=NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);

            //让线程自己结束
            for(int i=0;i<NUMBER;i++)
            {
                //唤醒 被阻塞的线程
                pthread_cond_signal(&pool->notEmpty);
            }

        }
    }

    return NULL;
}

template <typename T>
void ThreadPool<T>::threadExit()
{
    //获取当前的线程ID
    pthread_t tid=pthread_self();

    for(int i=0;i<maxNum;i++)
    {
        if(threadIDs[i]==tid)
        {
            //tid这个线程需要退出, 将该线程的 ID修改为0
            threadIDs[i]=0;
            cout<<"threadExit() called, "<<to_string(tid)<<" exiting...\n";

            break;
        }
    }

    //标准C函数
    pthread_exit(NULL);
}

//向线程池中添加任务
template <typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
    //添加任务不再需要锁了，因为所有的操作都是由 TaskQueue来控制了，不会发生冲突

    if(shutDown)
    {
        return;
    }

   //添加任务
   taskQ->addTask(task);

    //唤醒阻塞的线程  
    pthread_cond_signal(&notEmpty);   //唤醒消费者
}


//当前线程池中线程个数
template <typename T>
int ThreadPool<T>::getBusyNum()
{
    //这里有可能会被同时访问，所以需要加锁
    pthread_mutex_lock(&mutexPool);
    int busyNum=this->busyNum;
    pthread_mutex_unlock(&mutexPool);

    return busyNum;
}

//获取当前创建的线程个数
template <typename T>
int ThreadPool<T>::getAliveNum()
{
    pthread_mutex_lock(&mutexPool);
    int aliveNum=this->liveNum;
    pthread_mutex_unlock(&mutexPool);

    return aliveNum;
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    //关闭线程池
    shutDown=true;

    //回收阻塞管理者线程
    pthread_join(managerID,NULL);
    //唤醒阻塞的消费者线程
    for(int i=0;i<liveNum;i++)
    {
        pthread_cond_signal(&notEmpty);
    }

    //释放堆内存
    if(taskQ)
    {
        delete taskQ;
    }
    if(threadIDs)
    {
        delete []threadIDs;
    }
    
    //销毁互斥锁
    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&notEmpty);
}
