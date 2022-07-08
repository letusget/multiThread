#include "connectionMysql.h"

#include <iostream>
using namespace std;

//初始化数据库连接
MysqlConn::MysqlConn()
{
    //初始化连接环境
    m_conn=mysql_init(nullptr);
    //cout<<"5\n";

    //设置结果编码
    mysql_set_character_set(m_conn,"utf8");

}


//连接数据库
bool MysqlConn::connect(string user,string passwd,string dbName,string ip,unsigned short port)   //指定默认端口号
{
    //连接数据库
    cout<<"\n";
    cout<<"数据库连接信息如下: \n";
    cout<<ip.c_str()<<"\n";
    cout<<user.c_str()<<"\n";
    cout<<passwd.c_str()<<"\n";
    cout<<dbName.c_str()<<"\n";
    cout<<port<<"\n";
    cout<<"\n";
    MYSQL* ptr = mysql_real_connect(m_conn,ip.c_str(),user.c_str(), passwd.c_str(), dbName.c_str(),port,nullptr,0);
   // cout<<"6 "<<ptr<<"\n";
    //判断连接结果
    return ptr!=nullptr;
}
//更新数据库
bool MysqlConn::update(string sql)
{
    if(mysql_query(m_conn,sql.c_str()))
    {
        //mysql_query成功返回非0
        return false;
    }

    //mysql_query 语句执行成功
    return true;
}

//查询数据库
bool MysqlConn::query(string sql)
{
    //情况上次查询的结果
    freeResult();

    
    //将查询得到的结果集从服务器保存到 客户端
    if(mysql_query(m_conn,sql.c_str()))
    {
        //mysql_query成功返回非0
        return false;
    }

    //保存结果集
    m_result = mysql_store_result(m_conn);

    //mysql_query 语句执行成功
    return true;
}

//遍历查询得到的结果
bool MysqlConn::next()    //每调用一次，得到一个查询结果
{
    if(m_result!=nullptr)
    {
        //得到结果集中的一条记录
        m_row = mysql_fetch_row(m_result);  //返回一个地址

        if(m_row!=nullptr)
        {
            return true;
        }
    }

    return false;
}

//得到结果集中的字段
string MysqlConn::value(int index)   //取出结果集中对应索引的结果
{
    //获取结果中列的数量
    int colCount=mysql_num_fields(m_result);

    if(index>=colCount||index<0)
    {
        //有问题的返回空
        return string();
    }

    //得到字段值
    char* val=m_row[index];
    //防止出现二进制字符串导致 字符串结束
    unsigned long length = mysql_fetch_lengths(m_result)[index];

    //根据length构造，就不会因为/0 而提前结束了
    return string(val,length);
}

//事务操作：mysql默认自动提交
bool MysqlConn::transaction()
{
    //设置手动提交
    return mysql_autocommit(m_conn,false);
}

//提交事务
bool MysqlConn::commit()
{
    return mysql_commit(m_conn);
}

//事务回滚
bool MysqlConn::rollback()
{
    return mysql_rollback(m_conn);
}

void MysqlConn::freeResult()
{
    if(m_result)
    {
        mysql_free_result(m_result);
        m_result=nullptr;
    }
}

//刷新数据库连接的起始时间点
void MysqlConn::refreshAliveTime()
{
    m_aliveTime=steady_clock::now();
}

//计算连接存活时间
long long MysqlConn::getAliveTime()
{
    //时间差
    nanoseconds res=steady_clock::now()-m_aliveTime;    //纳秒
    //由高精度向低精度的转换，由纳秒转换为毫秒（数据丢失）
    milliseconds millsec=duration_cast<milliseconds>(res);

    //返回毫秒的个数
    return millsec.count();
}

//释放数据连接
MysqlConn::~MysqlConn()
{
    //释放mysql连接
    if(m_conn!=nullptr)
    {
        mysql_close(m_conn);
    }

    //
    freeResult();

}