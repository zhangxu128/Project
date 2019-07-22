/*=================================================
  数据管理模块 class DataManager;
	1、Init()			初始化
	2、GetDocs()		获取数据库中数据内容
	3、InsertDocs()     向数据库中插入数据
	4、DeleteDoc()      从数据库中删除数据
	5、Search()			从数据库中查找数据
  author :  zhangxu
===================================================*/


#pragma  once
#include "SqliteManager.h"

class DataManager {
public:
	static DataManager* GetInstance() {
		static DataManager damnagr;
		return &damnagr;
	}
	void Init();
	//得到数据库中的内容
	void GetDocs(std::string path, std::set<std::string>& dbset);

	//向数据库中插入数据
	void InsertDocs(const std::string path, const std::string doc);

	//在数据库中删除数据
	void DeleteDoc(const std::string& path, const std::string doc);

	//利用关键字进行查找
	void Search(std::string& key, std::vector<std::pair<std::string, std::string>>&doc_paths);

	//高亮显示关键字
	void SetHighLight(const std::string& doc_name, const std::string& key, std::string& prefix, std::string& highlight, std::string& suffix);

	~DataManager() {
	}
private:
	DataManager() {
		Init();
	};
	DataManager(const DataManager&) ;
	DataManager& operator=(const DataManager&) {};
	SqliteManager _sqlmagar;
};