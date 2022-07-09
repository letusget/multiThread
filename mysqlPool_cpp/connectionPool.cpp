#include "connectionPool.h"
#include <json/json.h>
#include <fstream>
#include <thread>

using namespace Json;

ConnectionPool::ConnectionPool()
{
    //加载配置文件
    if(!parseJsonFile())
    {
        return ;
    }

    //初始化 新的数据库连接
    for(int i=0;i<m_minSize;i++)
    {
        // MysqlConn* conn=new MysqlConn;
        // conn->connect(m_user,m_passwd,m_dbName,m_ip,m_port);
        // //放入连接队列
        // m_connectionQ.push(conn);
        //封装为函数
        addConnection();
    }

    //产生连接
    thread producer(&ConnectionPool::produceConnection,this);   //任务函数 指定当前实例对象
    //销毁连接
    thread recycler(&ConnectionPool::recycleConnection,this);    

    //线程分离，确保 主线程不会阻塞
    producer.detach();
    recycler.detach();

}

//初始化新的数据库连接
void ConnectionPool::addConnection()
{
    MysqlConn* conn=new MysqlConn;
    conn->connect(m_user,m_passwd,m_dbName,m_ip,m_port);
    //记录时间戳
    conn->refreshAliveTime();
    //放入连接队列
    m_connectionQ.push(conn);

}


//添加数据库连接
void ConnectionPool::produceConnection()
{
    //判断连接是否够用
    while (true)
    {
        //封装 互斥锁对象，由 unique_lock 自动管理
        unique_lock<mutex> locker(m_mutexQ);
        while (m_connectionQ.size()>=m_minSize)
        {
            //满足最小连接数，就阻塞 不在添加新的数据库连接
            m_cond.wait(locker);
        }

        //创建新的数据库连接
        addConnection();

        //唤醒 消费者
        m_cond.notify_all();
    }
    
}

//销毁数据库连接
void ConnectionPool::recycleConnection()
{
    while (true)
    {
        //定时执行，使用线程休眠
        //this_thread::sleep_for(chrono::seconds(1)); //休眠1S
        this_thread::sleep_for(chrono::milliseconds(500));

        //自动管理 连接队列
        lock_guard<mutex> locker(m_mutexQ);
        while (m_connectionQ.size()>m_minSize)
        {
            //判断空闲时长
            //取出队头连接
            MysqlConn* conn=m_connectionQ.front();
            //超过最大空闲时长就销毁
            if(conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connectionQ.pop();
                delete conn;
            }
            else
            {
                break;
            }
        }
        
    }
    
}

//获取该实现类
ConnectionPool* ConnectionPool::getConnectPool()
{
    //静态变量，紧跟进程
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::parseJsonFile()
{
    //读入文件
    ifstream ifs("dbconf.json");
    //读取流对象
    Reader rd;
    Value root; 
    //将读到的
    rd.parse(ifs,root);
    if(root.isObject())
    {
        //判断是否是JSON对象，如果是就读出所有json数据
        m_ip=root["ip"].asString(); //通过key获取到value
        m_port=root["port"].asInt();
        m_user=root["userName"].asString();
        m_passwd=root["password"].asString();
        m_dbName=root["dbName"].asString();
        m_minSize=root["minSize"].asInt();
        m_maxSize=root["maxSize"].asInt();
        m_maxIdleTime=root["maxIDleTime"].asInt();
        m_timeout=root["timeout"].asInt();

        return true;
    }
    return false;
}

//用户调用 数据库连接的接口
shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    //互斥锁
    unique_lock<mutex> locker(m_mutexQ);

    while(m_connectionQ.empty())
    {
        //连接队列为空，就需要 阻塞新的 连接请求
        if(cv_status::timeout == m_cond.wait_for(locker,chrono::milliseconds(m_timeout)))
        {
            //如果阻塞结束后 该线程没有被唤醒，就继续等待,直到取到可用的数据库连接
            if(m_connectionQ.empty())
            {
                continue;
            }   
        } 
    }

    //直到找到可用的数据库连接，取出该连接
    // MysqlConn* conn=m_connectionQ.front();
    //指定删除器，销毁连接但不析构
    shared_ptr<MysqlConn> connptr(m_connectionQ.front(),[this](MysqlConn* conn)
    {
        //互斥对象加锁
        lock_guard<mutex> locker(m_mutexQ); //缺陷：无法控制加锁的范围
        //更新时间戳
        conn->refreshAliveTime();
        m_connectionQ.push(conn);
    });
    m_connectionQ.pop();

    //唤醒生产者
    m_cond.notify_all();

    //return conn;
    return connptr; //通过智能指针管理 数据库连接资源
}

ConnectionPool::~ConnectionPool()
{
    //判断任务队列中的数据，析构连接队列
    while(!m_connectionQ.empty())
    {
        MysqlConn* conn=m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }

}