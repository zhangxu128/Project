#include "DataManager.h"

//初始化建表
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

//获取查询到的文件信息
void DataManager::GetDocs(std::string path, std::set<std::string>& dbset) {
	//这里是得到所有的文件名字和目录名字
	char ch[4096];
	memset(ch, 0, sizeof(ch));
	sprintf(ch, "select doc_name from doc_db where doc_path='%s';", path.c_str());
	std::string sql(ch);
	int row;
	int col;
	char** ppRet;
	AutoGetTable s(&_sqlmagar, sql, row, col, ppRet);
	//得到结果ppRet
	for (int i = 1; i <= row; i++) {
		dbset.insert(ppRet[i]);
	}
}

//向数据库中插入数据
void DataManager::InsertDocs(const std::string path, const std::string doc) {
	//将路径和doc插入到我们的数据库中
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

//本地文件销毁，删除数据库中对应的数据
void DataManager::DeleteDoc(const std::string& path, const std::string doc) {
	char ch[1024];
	memset(ch, 0, sizeof(ch));
	//先删除这个文件，有可能是目录，如果是目录就要目录下面所有的东西
	sprintf(ch, "delete from doc_db where doc_name='%s' and doc_path='%s';", doc.c_str(), path.c_str());
	std::string sql(ch);
	_sqlmagar.ExecuteSql(sql);
	char ch1[256];
	memset(ch1, 0, sizeof(ch1));
	sprintf(ch1, "delete from doc_db where doc_path like '%s%%';", (path + "\\" + doc).c_str());
	std::string sql1(ch1);
	_sqlmagar.ExecuteSql(sql1);
}

//根据关键字在数据库中检索
void DataManager::Search(std::string& key, std::vector<std::pair<std::string, std::string>>&doc_paths) {
	//使用关键字得到数据，使用一个pair对组来存储数据，第一个是name，第二个是path
	
	char ch[256];
	memset(ch, 0, sizeof(ch));
	std::string pinyin;
	ChineseConvertPy(key, pinyin);
	std::string pinyin_initials = ChineseConvertPyInit(key.c_str());
	sprintf(ch, "select doc_name,doc_path from doc_db where doc_name_pinyin \
								 like '%%%s%%' or doc_name_initials like '%%%s%%'COLLATE NOCASE;", pinyin.c_str(),pinyin_initials.c_str());
	//使用AutoGetTable来得到数据
	std::string sql(ch);
	int row;
	int col;
	char ** ppRet;
	//得到数据
	AutoGetTable agt(&_sqlmagar, sql, row, col, ppRet);
	for (int i = 1; i <= row; i++) {
		//i*2+0和i*2+1 这两个位置就是我们需要的位置
		std::pair<std::string, std::string> p;
		p.first = ppRet[i * 2];
		p.second = ppRet[i * 2 + 1];
		doc_paths.push_back(p);
	}
}

//设置关键字高亮
void DataManager::SetHighLight(const std::string& doc_name, const std::string& key, std::string& prefix, std::string& highlight, std::string& suffix) {
	
	//直接利用中文搜索高亮关键字
	{
		int pos = doc_name.find(key);
		if (pos != std::string::npos) {
			prefix = doc_name.substr(0, pos);
			highlight = key;
			suffix = doc_name.substr(pos + key.size(), std::string::npos);

			return;
		}
	}
	//输入拼音全拼，高亮对应汉字
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
				//输入的是ASCLL
				if (doc_name[wd_index] >= 0 && doc_name[wd_index] <= 127) {
					py_index++;
					wd_index++;
				}
				//汉字
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

	//输入首字母
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
				//输入的是ASCLL
				if (doc_name[wd_index] >= 0 && doc_name[wd_index] <= 127) {
					py_inials_index++;
					wd_index++;
				}
				//汉字
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
