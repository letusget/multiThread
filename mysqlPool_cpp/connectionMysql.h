#ifndef CONNECTIONMYSQL_H
#define CONNECTIONMYSQL_H

#include <string>
#include <mysql.h>
#include <chrono>

//chrono 需要std标准库
//using namespace std::chrono;
using namespace std;
using namespace chrono;

class MysqlConn
{
private:
    MYSQL* m_conn=nullptr;  //初始化连接
    MYSQL_RES* m_result=nullptr;    //保存连接结果集
    MYSQL_ROW m_row=nullptr;    //保存结果集的一条记录(指向结果集)

    //存放连接时长
    steady_clock::time_point m_aliveTime;
    //释放结果集内存
    void freeResult();
public:
    //初始化数据库连接
    MysqlConn();
    
    //释放数据连接
    ~MysqlConn();

    //连接数据库
    bool connect(string user,string passwd,string dbName,string ip,unsigned short port=3306);   //指定默认端口号
    //更新数据库
    bool update(string sql);

    //查询数据库
    bool query(string sql);

    //遍历查询得到的结果
    bool next();    //每调用一次，得到一个查询结果

    //得到结果集中的字段
    string value(int index);    //取出结果集中对应索引的结果

    //事务操作：mysql默认自动提交
    bool transaction();

    //提交事务
    bool commit();

    //事务回滚
    bool rollback();

    //刷新数据库连接的起始时间点
    void refreshAliveTime();
    //计算连接存活时间
    long long getAliveTime();
};

#endif