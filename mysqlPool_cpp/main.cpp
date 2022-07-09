#include <iostream>
#include <memory>

#include "connectionMysql.h"
#include "connectionPool.h"

#include <thread>

using namespace std;

//测试数据库连接池性能,不使用数据库连接池
//单线程 5000个连接用时为: 51036304657 纳秒   即 51036 毫秒
//多线程 5000个连接用时为: 21062800477 纳秒   即 21062 毫秒
void op1(int begin,int end)
{
    //创建数据库连接，并进行数据插入
    for(int i=begin;i<end;i++)
    {
        MysqlConn conn;
        //连接数据库
        conn.connect("root","159612","book","192.168.18.135");
        char sql[1024]={0};
        //插入
        sprintf(sql,"insert into t_user values(%d,'test','test','test@163.com')",i);
        //执行sql语句
        conn.update(sql);
    }
}

//测试数据库连接池性能,使用数据库连接池
//单线程 5000个连接用时为: 9164084935 纳秒    即 9164 毫秒
//多线程 5000个连接用时为: 2762778513 纳秒   即 2762 毫秒
void op2(ConnectionPool* pool, int begin,int end)   //传入数据库连接池实例
{
    //创建数据库连接，并进行数据插入
    for(int i=begin;i<end;i++)
    {
        //由 shared_ptr 调用数据库连接池的析构函数，实现资源回收
        shared_ptr<MysqlConn> conn=pool->getConnection();
        char sql[1024]={0};

        //插入
        sprintf(sql,"insert into t_user values(%d,'test','test','test@163.com')",i);
        conn->update(sql);
    }
}

void testMysql1()
{
    //条件编译
#if 1
    //单线程，不使用数据库连接池
    steady_clock::time_point begin=steady_clock::now();
    op1(0,5000);
    steady_clock::time_point end=steady_clock::now();

    //精确到纳秒的 时间段duration
    auto length = end - begin;
    cout<<" 非数据库连接池，单线程情况下，5000个连接用时为: "<<length.count()<<" 纳秒   "
        <<"即 "<<length.count()/1000000<<" 毫秒\n";
# else
    //单线程，使用数据库连接池
    ConnectionPool* pool=ConnectionPool::getConnectPool();
    steady_clock::time_point begin=steady_clock::now();
    op2(pool, 0, 5000);
    steady_clock::time_point end=steady_clock::now();

    // //精确到纳秒的 时间段duration
    auto length = end - begin;
    cout<<" 使用数据库连接池，单线程情况下，5000个连接用时为: "<<length.count()<<" 纳秒    "
        <<"即 "<<length.count()/1000000<<" 毫秒\n";
# endif
}

void testMysql2()
{
# if 1

    //5000个连接用时为: 21062800477 纳秒   即 21062 毫秒

    //在创建子线程时，添加一个额外的数据库连接，这样就不会因为多个线程同时连接数据库，数据库丢弃连接了
    MysqlConn conn;
    conn.connect("root","159612","book","192.168.18.135");
    
    steady_clock::time_point begin=steady_clock::now();
    //添加线程
    thread t1(op1,0,1000);
    thread t2(op1,1000,2000);
    thread t3(op1,2000,3000);
    thread t4(op1,3000,4000);
    thread t5(op1,4000,5000);
    //阻塞主线程
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end=steady_clock::now();
    //精确到纳秒的 时间段duration
    auto length = end - begin;
    cout<<" 非数据库连接池，多线程情况下，5000个连接用时为: "<<length.count()<<" 纳秒   "
        <<"即 "<<length.count()/1000000<<" 毫秒\n";
# else

    //5000个连接用时为: 2762778513 纳秒   即 2762 毫秒

    ConnectionPool* pool = ConnectionPool::getConnectPool();
    steady_clock::time_point begin=steady_clock::now();
    //添加线程
    thread t1(op2,pool,0,1000);
    thread t2(op2,pool,1000,2000);
    thread t3(op2,pool,2000,3000);
    thread t4(op2,pool,3000,4000);
    thread t5(op2,pool,4000,5000);
    //阻塞主线程
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end=steady_clock::now();
    //精确到纳秒的 时间段duration
    auto length = end - begin;
    cout<<" 数据库连接池，多线程情况下，5000个连接用时为: "<<length.count()<<" 纳秒   "
        <<"即 "<<length.count()/1000000<<" 毫秒\n";
# endif
}

void query()
{
    MysqlConn conn;

    //连接数据库
    conn.connect("root","159612","book","192.168.18.135");

    //插入
    string insertSql="insert into t_user values(3,'test','test','test@163.com')";

    //执行sql语句
    bool flag = conn.update(insertSql);
    cout<<"Insert SQL flag value is "<<flag<<"\n";

    //查询
    string querySql="select * from t_user";
    conn.query(querySql);
    //遍历得到的结果集
    while (conn.next())
    {
        cout<<conn.value(0)<<", " <<conn.value(1)<<", "<<conn.value(2)<<", "<<conn.value(3)<<"\n";

    }
    
}
int main()
{
    //测试插入查询数据
    // query();

    //测试性能 单线程下使用功能 数据库连接池与不使用数据库连接池
    testMysql1();

    // 多线程下使用数据库连接池与不使用数据库连接池
    //testMysql2();
    return 0;
}