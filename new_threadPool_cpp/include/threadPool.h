#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "TaskQueue.h"

template <typename T>
class ThreadPool {
   private:
    //任务队列
    TaskQueue<T> *taskQ;

    //管理者线程
    pthread_t managerID;  //管理者线程ID
    //工作者线程
    pthread_t *threadIDs;  //工作的线程ID

    //线程数: 指定线程数的范围
    int minNum;
    int maxNum;

    //当前工作的线程数
    int busyNum;
    //当前存活的线程数，已创建的线程个数
    int liveNum;
    //空闲的线程，方便在任务较少时杀死这些线程
    int exitNum;

    //防止线程访问冲突，需要加锁
    std::mutex mutexPool;              //线程池的锁
    std::condition_variable notEmpty;  //判断是否空

    //每次增加的线程个数
    static const int NUMBER = 2;

    //销毁线程
    bool shutDown;  //销毁线程池为1，不销毁为0

   public:
    //创建线程池并初始化: 最大和最小线程数以及任务队列的容量
    ThreadPool(int min, int max);

    //销毁线程池
    ~ThreadPool();

    //向线程池中添加任务
    void addTask(Task<T> task);

    //当前线程池中线程个数
    int getBusyNum();

    //获取当前创建的线程个数
    int getAliveNum();

   private:
    //工作函数
    static void *worker(void *arg);
    static void *manager(void *arg);
    void threadExit();
};

#endif