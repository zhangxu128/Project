/*=================================================
  ���ݿ����ģ�� class SqliteManager;
	1��Open()		   �������ݿ�
	2��Close()         �ر����ݿ�����
	3��ExcuteSql       ִ��sql���
	4��GetTable()      ���ز��ҵ���������Ϣ
  ��װRAII       class AutoGetTable;
     �Զ��ͷſ��ٵ��ڴ�ռ䣬�����ڴ�й©��
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
	//�������ݿ�
	void Open(const std::string& path);

	//�ر����ݿ�����
	void Close();

	//ִ��sql���
	void ExecuteSql(const std::string sql);

	//�����ݿ���в��Ҳ������õ�����һ����ά�����ָ��,��ʵ����һ��һ����ŵģ��ö�ά�����ʾ��ʵ������һά����
	void GetTable(const std::string sql, int &row, int &col, char**&ppRet);

	//��ֹ�����͸�ֵ
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	sqlite3* _db;
	std::mutex _mutx;
};



//ʹ��ARII��GetTable��װ�����ڶ��ڴ���й����Զ��ͷŷ��صĶ�ά����
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