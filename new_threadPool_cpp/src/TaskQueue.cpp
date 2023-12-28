#include "TaskQueue.h"

template <typename T> TaskQueue<T>::TaskQueue(/* args */) {
  //初始化互斥锁
}

//添加任务
template <typename T> void TaskQueue<T>::addTask(Task<T> task) {
  m_mutex.lock();
  //操作对尾 添加任务
  m_taskQ.push(task);
  m_mutex.unlock();
}

template <typename T> void TaskQueue<T>::addTask(callBack f, void *arg) {
  m_mutex.lock();
  //操作对尾 添加任务
  m_taskQ.push(Task<T>(f, arg));
  m_mutex.unlock();
}

//取出一个任务 -直接操作队头即可
template <typename T> Task<T> TaskQueue<T>::takeTask() {
  Task<T> t;
  m_mutex.lock();
  //首先判断任务队列是否为空
  if (!m_taskQ.empty()) {
    t = m_taskQ.front();
    m_taskQ.pop();
  }
  m_mutex.unlock();
  return t;
}

template <typename T> TaskQueue<T>::~TaskQueue() {
  //销毁互斥锁
}