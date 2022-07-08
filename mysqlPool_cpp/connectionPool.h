#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>   //条件变量

#include "connectionMysql.h"

using namespace std;

//懒汉模式
class ConnectionPool
{
private:
    ConnectionPool();

    //数据库连接信息
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    //指定最大和最小连接数
    int m_maxSize;
    int m_minSize;

    //超时时长
    int m_timeout;  //等待数据库连接的时长，如果线程繁忙，可以让该 连接 等待的时长
    //最大空闲时长
    int m_maxIdleTime;  //有太多空闲的有效连接，销毁空闲时间最长的空闲连接
    //连接队列
    queue<MysqlConn*> m_connectionQ;
    //防止线程冲突
    mutex m_mutexQ;
    //条件变量用于 阻塞线程
    condition_variable m_cond;

    //解析JSON配置
    bool parseJosnFile();
    //添加数据库连接
    void produceConnection();
    //销毁数据库连接
    void recycleConnection();
    //初始化新的数据库连接
    void addConnection();

public:
    //获取该实现类
    static ConnectionPool* getConnectPool();
    //防止拷贝构造
    ConnectionPool(const ConnectionPool& obj)=delete;
    //防止赋值构造
    ConnectionPool& operator=(const ConnectionPool& obj)=delete;

    //用户调用 数据库连接的接口
    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
};



#endif