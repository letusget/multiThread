#include "TaskQueue.h"

template <typename T>
TaskQueue<T>::TaskQueue(/* args */)
{
    //初始化互斥锁
    pthread_mutex_init(&m_mutex,nullptr);
}

//添加任务
template <typename T>
void TaskQueue<T>::addTask(Task<T> task)
{
    pthread_mutex_lock(&m_mutex);
    //操作对尾 添加任务
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}

template <typename T>
void TaskQueue<T>::addTask(callBack f,void* arg)
{
    pthread_mutex_lock(&m_mutex);
    //操作对尾 添加任务
    m_taskQ.push(Task<T>(f,arg));
    pthread_mutex_unlock(&m_mutex);
}

//取出一个任务 -直接操作队头即可
template <typename T>
Task<T> TaskQueue<T>::takeTask()
{
    Task<T> t;
    pthread_mutex_lock(&m_mutex);
    //首先判断任务队列是否为空
    if(!m_taskQ.empty())
    {
        t=m_taskQ.front();
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}

template <typename T>
TaskQueue<T>::~TaskQueue()
{
    //销毁互斥锁
    pthread_mutex_destroy(&m_mutex);
}