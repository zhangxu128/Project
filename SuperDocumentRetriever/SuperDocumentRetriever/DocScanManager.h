/*=================================================
  数据扫描模块 class DocScanManager;
	1、Scan()		扫描用于本地及数据库中数据比对
	2、DirectoryList()    遍历指定路径下的文件信息
  author :  zhangxu
===================================================*/

#pragma  once
#include "DataManager.h"
#include <io.h>

class DocScanManager {
public:

	static DocScanManager* GetInstance() {
		static DocScanManager s;
		return &s;
	}
	void MonitorScan(const std::string& path);
	void Scan(const std::string& path);
private:		 
	//维护一个数据管理模块的对象，用于比对本地数据与数据库中数据，从而达到更新数据库的目的。
	//DataManager _dbmagar;
	DocScanManager(){};
	DocScanManager(const DocScanManager&);
	DocScanManager& operator=(const DocScanManager&) {};
};


//遍历指定路径下的文件信息
static void DirectoryList(const std::string& path, std::vector<std::string>&subfiles, \
	std::vector<std::string>&subdirs) {
	_finddata_t file;//定义一个文件结构体
	//此时的路径是需要改变的，需要遍历该目录下面的，传递进来的只是到底此目录文件
	std::string _path = path + "\\*.*";//

	intptr_t handle = _findfirst(_path.c_str(), &file);
	if (handle == -1) {
		ERROE_LOG("_findfirst:%s", _path.c_str());
	}
	do {
		// _A_SUBDIR（文件夹）就是目录，否则就是文件,name就是file的名字属性，是一个数组，长度是256。
		if ((file.attrib & _A_SUBDIR) && !(file.attrib&_A_HIDDEN)) {
			//目录的时候需要判断是不是.或者..，如果包含了这两个文件在查询的时候就会递归死循环
			if ((strcmp(file.name, ".") != 0) && (strcmp(file.name, "..") != 0)) {
				subdirs.push_back(file.name);
			}
		}
		else {
			//此时就是文件了
			subfiles.push_back(file.name);
		}

	} while (_findnext(handle, &file) == 0);

	_findclose(handle);
}