#include <iostream>
#include <memory>

#include "connectionMysql.h"
#include "connectionPool.h"

using namespace std;

void query()
{
    MysqlConn conn;

    //连接数据库
    //cout<<"1\n";
    conn.connect("root","159612","book","192.168.18.135");

    //插入
    //cout<<"2\n";
    string insertSql="insert into t_user values(3,'test','test','test@163.com')";

    //执行sql语句
    //cout<<"3\n";
    bool flag = conn.update(insertSql);
    cout<<"Insert SQL flag value is "<<flag<<"\n";

    //查询
    //cout<<"4\n";
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
    query();

    return 0;
}