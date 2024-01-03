#include <chrono>
#include <iostream>
#include <thread>

#include "Task.h"
#include "TaskQueue.h"
#include "threadPool.cpp"  //由于模板类的声明和实现是分开的，所以需要包含源文件，否则就会报错“未定义的引用”
#include "threadPool.h"

//工作函数
void taskFunction(void *arg) {
    //需要对传入的地址进行类型转换，然后再解引用，得到该地址保存的数据
    int num = *(int *)arg;
    // printf("thread %ld is working, number = %d \n",pthread_self(),num);
    std::cout << "thread " << pthread_self() << " is working, number= " << num << "\n";

    std::chrono::milliseconds dura(1000);
    std::this_thread::sleep_for(dura);
}

int main() {
    std::cout << "threadpool 向你问好! "
              << "\n";

    int minThread = 3;
    int maxThread = 10;
    int tasks     = 100;
    //创建线程池
    ThreadPool<int> pool(minThread,
                         maxThread);  //当线程执行完毕会自动析构，所以这里就不再需要主动调用析构函数了

    //添加100个任务
    //使用堆 内存
    // int * num=(int *)malloc(sizeof(int));
    for (int i = 0; i < tasks; i++) {
        //使用堆 内存
        int *num = new int(i + 100);  //这里的空间在work中释放了，不存在内存泄露
        pool.addTask(Task<int>(taskFunction, num));
    }

    //保证工作线程已经处理完毕
    std::chrono::milliseconds dura(10000);
    std::this_thread::sleep_for(dura);

    return 0;
}
