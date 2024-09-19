#include "sql_pool.h"

// Sql_pool *Sql_pool::GetInstance() {
// 	if (instance == nullptr) {
//         instance = new Sql_pool();
//     }
//     return instance;
// }

std::string Sql_pool::url = "localhost";
std::string Sql_pool::user = "root";
std::string Sql_pool::password = "2000416Czr!";
std::string Sql_pool::database = "user";
int Sql_pool::port = 3306;
std::queue<MYSQL *> Sql_pool::sqlList;
std::mutex Sql_pool::mutexPool;


void Sql_pool::init() {
    for (int i = 0; i < 8; i++) {
		MYSQL *con = NULL;
		con = mysql_init(con);
		con = mysql_real_connect(con, url.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0);
		sqlList.push(con);
	}
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *Sql_pool::GetConnection() {
	MYSQL *con = NULL;

	if (0 == sqlList.size())
		return NULL;
	
	std::unique_lock<std::mutex> lk(mutexPool);

	con = sqlList.front();
	sqlList.pop();

	lk.unlock();
	return con;
}

//释放当前使用的连接
bool Sql_pool::ReleaseConnection(MYSQL *con) {
	if (NULL == con)
		return false;

	std::unique_lock<std::mutex> lk(mutexPool);

	sqlList.push(con);

	lk.unlock();
	return true;
}
