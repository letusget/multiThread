#ifndef QTASKQUEUE_H
#define QTASKQUEUE_H

#include "Task.h"
#include <queue>
// #include <pthread.h>
#include <thread>

//任务队列
template <typename T>
class TaskQueue
{
private:
    std::queue<Task<T>> m_taskQ;

    //使用 互斥锁，防止同时访问冲突
    pthread_mutex_t m_mutex;

public:
    TaskQueue();

    //添加任务
    void addTask(Task<T> task);
    void addTask(callBack f,void* arg);

    //取出一个任务 -直接操作队头即可
    Task<T> takeTask();

    //获取当前任务数量
    inline size_t taskNumber()  //与返回值的类型一致
    {
        //使用内联函数，提高效率，直接获取任务个数
        return m_taskQ.size();
    }

    ~TaskQueue();
};




#endif