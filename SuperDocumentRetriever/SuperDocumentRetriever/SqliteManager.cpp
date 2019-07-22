#include "SqliteManager.h"

//�������ݿ�
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

//ִ��sql���
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

//���ؼ�����Ϣ
void SqliteManager::GetTable(const std::string sql, int &row, int &col, char**&ppRet) {
	//��ʱ���ܵ����ڴ��й©����ʱ���ǽ�GetTable��װ����
	//��������ppRet���Ƿ��ص�ֵ��ʹ��ARII���Ƿ�ֹ��;�����쳣�������
	//��ʱû���ͷŵ��ڴ棬���Բ���ARII������
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

//�ر����ݿ�����
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

