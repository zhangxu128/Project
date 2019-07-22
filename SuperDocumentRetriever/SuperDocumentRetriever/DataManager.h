/*=================================================
  ���ݹ���ģ�� class DataManager;
	1��Init()			��ʼ��
	2��GetDocs()		��ȡ���ݿ�����������
	3��InsertDocs()     �����ݿ��в�������
	4��DeleteDoc()      �����ݿ���ɾ������
	5��Search()			�����ݿ��в�������
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
	//�õ����ݿ��е�����
	void GetDocs(std::string path, std::set<std::string>& dbset);

	//�����ݿ��в�������
	void InsertDocs(const std::string path, const std::string doc);

	//�����ݿ���ɾ������
	void DeleteDoc(const std::string& path, const std::string doc);

	//���ùؼ��ֽ��в���
	void Search(std::string& key, std::vector<std::pair<std::string, std::string>>&doc_paths);

	//������ʾ�ؼ���
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