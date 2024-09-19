#ifndef SQL_POOL_H
#define SQL_POOL_H

#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <string>
#include <mysql/mysql.h>


class Sql_pool {
public:
    static MYSQL *GetConnection();				 //获取数据库连接

    //单例模式
	// static Sql_pool *GetInstance();
    static bool ReleaseConnection(MYSQL *);

    static void init();

private:
    // static Sql_pool* instance;//静态成员变量，类内声明，类外初始化
    static std::string url;
    static std::string user;
    static std::string password;
    static std::string database;
    static int port;
    static std::queue<MYSQL *> sqlList;   //  数据库连接池队列
    static std::mutex mutexPool;   //  工作队列的锁
    // static std::condition_variable cond;   // 条件变量
};

#endif