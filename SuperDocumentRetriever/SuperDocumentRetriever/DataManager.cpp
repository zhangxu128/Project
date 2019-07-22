#include "DataManager.h"

//��ʼ������
void DataManager::Init() {
	_sqlmagar.Open("doc.db");
	std::string sql = "CREATE TABLE if not exists doc_db( \
		ID INTEGER PRIMARY KEY AUTOINCREMENT,\
		doc_name	  TEXT    NOT NULL, \
		doc_path	  TEXT     NOT NULL, \
		doc_name_pinyin       text, \
		doc_name_initials    text );";
	_sqlmagar.ExecuteSql(sql);
}

//��ȡ��ѯ�����ļ���Ϣ
void DataManager::GetDocs(std::string path, std::set<std::string>& dbset) {
	//�����ǵõ����е��ļ����ֺ�Ŀ¼����
	char ch[4096];
	memset(ch, 0, sizeof(ch));
	sprintf(ch, "select doc_name from doc_db where doc_path='%s';", path.c_str());
	std::string sql(ch);
	int row;
	int col;
	char** ppRet;
	AutoGetTable s(&_sqlmagar, sql, row, col, ppRet);
	//�õ����ppRet
	for (int i = 1; i <= row; i++) {
		dbset.insert(ppRet[i]);
	}
}

//�����ݿ��в�������
void DataManager::InsertDocs(const std::string path, const std::string doc) {
	//��·����doc���뵽���ǵ����ݿ���
	char ch[1024];
	memset(ch, 0, sizeof(ch));
	std::string piny;
	int ret = ChineseConvertPy(doc, piny);
	if (ret != 0) {
		ERROE_LOG("ChineseConvertPY error!!!");
	}
	std::string name_init;
	name_init = ChineseConvertPyInit(doc.c_str());
	const std::string s;
	sprintf(ch, "insert into doc_db (doc_name,doc_path,doc_name_pinyin,doc_name_initials) values \
								 ('%s','%s','%s','%s');", doc.c_str(), path.c_str(), piny.c_str(), name_init.c_str());
	std::string sql(ch);
	_sqlmagar.ExecuteSql(sql);
}

//�����ļ����٣�ɾ�����ݿ��ж�Ӧ������
void DataManager::DeleteDoc(const std::string& path, const std::string doc) {
	char ch[1024];
	memset(ch, 0, sizeof(ch));
	//��ɾ������ļ����п�����Ŀ¼�������Ŀ¼��ҪĿ¼�������еĶ���
	sprintf(ch, "delete from doc_db where doc_name='%s' and doc_path='%s';", doc.c_str(), path.c_str());
	std::string sql(ch);
	_sqlmagar.ExecuteSql(sql);
	char ch1[256];
	memset(ch1, 0, sizeof(ch1));
	sprintf(ch1, "delete from doc_db where doc_path like '%s%%';", (path + "\\" + doc).c_str());
	std::string sql1(ch1);
	_sqlmagar.ExecuteSql(sql1);
}

//���ݹؼ��������ݿ��м���
void DataManager::Search(std::string& key, std::vector<std::pair<std::string, std::string>>&doc_paths) {
	//ʹ�ùؼ��ֵõ����ݣ�ʹ��һ��pair�������洢���ݣ���һ����name���ڶ�����path
	
	char ch[256];
	memset(ch, 0, sizeof(ch));
	std::string pinyin;
	ChineseConvertPy(key, pinyin);
	std::string pinyin_initials = ChineseConvertPyInit(key.c_str());
	sprintf(ch, "select doc_name,doc_path from doc_db where doc_name_pinyin \
								 like '%%%s%%' or doc_name_initials like '%%%s%%'COLLATE NOCASE;", pinyin.c_str(),pinyin_initials.c_str());
	//ʹ��AutoGetTable���õ�����
	std::string sql(ch);
	int row;
	int col;
	char ** ppRet;
	//�õ�����
	AutoGetTable agt(&_sqlmagar, sql, row, col, ppRet);
	for (int i = 1; i <= row; i++) {
		//i*2+0��i*2+1 ������λ�þ���������Ҫ��λ��
		std::pair<std::string, std::string> p;
		p.first = ppRet[i * 2];
		p.second = ppRet[i * 2 + 1];
		doc_paths.push_back(p);
	}
}

//���ùؼ��ָ���
void DataManager::SetHighLight(const std::string& doc_name, const std::string& key, std::string& prefix, std::string& highlight, std::string& suffix) {
	
	//ֱ�������������������ؼ���
	{
		int pos = doc_name.find(key);
		if (pos != std::string::npos) {
			prefix = doc_name.substr(0, pos);
			highlight = key;
			suffix = doc_name.substr(pos + key.size(), std::string::npos);

			return;
		}
	}
	//����ƴ��ȫƴ��������Ӧ����
	{
		std::string doc_pinyin;
		ChineseConvertPy(doc_name,doc_pinyin);
		std::string doc_key;
		ChineseConvertPy(key, doc_key);
		
		int wd_index = 0,wd_start = 0;
		int wd_len = 0;
		int py_index = 0;
		int py_start = doc_pinyin.find(doc_key);
		int py_end = py_start + doc_key.size();
		
		if (py_start != std::string::npos) {
			while (py_index < py_end) {
				if (py_index == py_start) {
					wd_start = wd_index;
				}
				//�������ASCLL
				if (doc_name[wd_index] >= 0 && doc_name[wd_index] <= 127) {
					py_index++;
					wd_index++;
				}
				//����
				else {
					char chinese[3] = { 0 };
					chinese[0] = doc_name[wd_index];
					chinese[1] = doc_name[wd_index + 1];
					wd_index += 2;

					std::string chineses;
					ChineseConvertPy(chinese, chineses);
					py_index += chineses.size();

				}
			}
			wd_len = wd_index - wd_start;
			prefix = doc_name.substr(0, wd_start);
			highlight = doc_name.substr(wd_start,wd_len);
			suffix = doc_name.substr(wd_start+wd_len, std::string::npos);

			return;
		}
	}

	//��������ĸ
	{
		std::string doc_name_initials = ChineseConvertPyInit(doc_name.c_str());
		std::string doc_key_initials = ChineseConvertPyInit(key.c_str());
		
		int wd_index = 0, wd_start = 0;
		int wd_len = 0;
		int py_inials_index = 0;
		int py_inials_start = doc_name_initials.find(doc_key_initials);
		int py_end = py_inials_start + doc_key_initials.size();

		if (py_inials_start != std::string::npos) {
			while (py_inials_index < py_end) {
				if (py_inials_index == py_inials_start) {
					wd_start = wd_index;
				}
				//�������ASCLL
				if (doc_name[wd_index] >= 0 && doc_name[wd_index] <= 127) {
					py_inials_index++;
					wd_index++;
				}
				//����
				else {
					char chinese[3] = { 0 };
					chinese[0] = doc_name[wd_index];
					chinese[1] = doc_name[wd_index + 1];
					wd_index += 2;

					std::string chineses;
					ChineseConvertPy(chinese, chineses);
					py_inials_index ++;

				}
			}
			wd_len = wd_index - wd_start;
			prefix = doc_name.substr(0, wd_start);
			highlight = doc_name.substr(wd_start, wd_len);
			suffix = doc_name.substr(wd_start + wd_len, std::string::npos);

			return;
		}
	}

}
