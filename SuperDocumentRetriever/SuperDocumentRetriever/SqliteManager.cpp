#include "SqliteManager.h"

//连接数据库
void SqliteManager::Open(const std::string& path) {
	int ret;
	{
		std::lock_guard<std::mutex> lock(_mutx);
		ret = sqlite3_open(path.c_str(), &_db);
	}
	if (ret != SQLITE_OK) {
		ERROE_LOG("sqlite3_open:%s error", path);
	}
	else {
		TRACE_LOG("sqlite3_open:%s success", path);
	}
}

//执行sql语句
void SqliteManager::ExecuteSql(const std::string sql) {
	char* msg = nullptr;
	int ret;
	{
		std::lock_guard<std::mutex> lock(_mutx);
		ret = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &msg);
	}
	if (ret != SQLITE_OK) {
		ERROE_LOG("ExecuteSql error:%s", sql, msg);
	}
	else {
		TRACE_LOG("ExecuteSql:%s success", sql.c_str());
	}
}

//返回检索信息
void SqliteManager::GetTable(const std::string sql, int &row, int &col, char**&ppRet) {
	//此时可能导致内存的泄漏，此时我们将GetTable封装起来
	//返回来的ppRet就是返回的值，使用ARII就是防止中途出现异常的情况，
	//此时没有释放掉内存，所以采用ARII来进行
	char* errmsg = nullptr;
	int ret;
	{
		std::lock_guard<std::mutex> lock(_mutx);
		ret = sqlite3_get_table(_db, sql.c_str(), &(ppRet), &row, &col, &errmsg);
	}
	if (ret != SQLITE_OK) {
		ERROE_LOG("GetTable error: %s", sql.c_str());
	}
	else {
		TRACE_LOG("GetTable success: %s", sql);
	}
}

//关闭数据库连接
void SqliteManager::Close() {
	int ret;
	{
		std::lock_guard<std::mutex> lock(_mutx);
		ret = sqlite3_close(_db);
	}
	if (ret != SQLITE_OK) {
		ERROE_LOG("sqlite3_close: error");
	}
	else {
		TRACE_LOG("sqlite3_close success");
	}
}

