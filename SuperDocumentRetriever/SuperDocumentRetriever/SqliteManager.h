/*=================================================
  数据库管理模块 class SqliteManager;
	1、Open()		   连接数据库
	2、Close()         关闭数据库连接
	3、ExcuteSql       执行sql语句
	4、GetTable()      返回查找到的数据信息
  封装RAII       class AutoGetTable;
     自动释放开辟的内存空间，避免内存泄漏。
  author :  zhangxu
===================================================*/

#pragma once
#include "Common.h"
#include <sqlite3.h>
#include <mutex>

class SqliteManager {
public:
	SqliteManager()
		:_db(nullptr)
	{}
	~SqliteManager() {
		Close();
	}
	//连接数据库
	void Open(const std::string& path);

	//关闭数据库连接
	void Close();

	//执行sql语句
	void ExecuteSql(const std::string sql);

	//对数据库进行查找操作，得到的是一个二维数组的指针,其实就是一个一个存放的，用二维数组表示，实际上是一维数组
	void GetTable(const std::string sql, int &row, int &col, char**&ppRet);

	//防止拷贝和赋值
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	sqlite3* _db;
	std::mutex _mutx;
};



//使用ARII将GetTable封装，便于对内存进行管理，自动释放返回的二维数组
class AutoGetTable {
public:
	AutoGetTable(SqliteManager* dbmagar, const std::string sql, int & row, int & col, char**& ppRet)
		:_dbMag(dbmagar), _ppVlaue(0)
	{
		_dbMag->GetTable(sql, row, col, ppRet);
		_ppVlaue = ppRet;
	}
	virtual ~AutoGetTable() {
		if (_ppVlaue) {
			sqlite3_free_table(_ppVlaue);
		}
	}

private:
	SqliteManager* _dbMag;
	char ** _ppVlaue;
};