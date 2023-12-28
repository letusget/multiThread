#ifndef TASK_H
#define TASK_H

//任务队列中的任务描述，包含一个函数和函数的参数
using namespace std;

using callBack = void (*)(void *arg);

//任务队列中的任务描述
template <typename T> struct Task {
  callBack function;
  T *arg;

  //无参构造函数
  Task() {
    function = nullptr;
    arg = nullptr;
  }

  //有参构造
  Task(callBack f, void *arg) {
    function = f;
    this->arg = (T *)arg;
  }
};

#endif