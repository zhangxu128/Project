#include "DocScanManager.h"

void DocScanManager::Scan(const std::string& path) {
	//通过我们path路径来取得目录下面的数据和数据库中的数据
	std::set<std::string>localset;
	std::vector<std::string>localdirs;
	std::vector<std::string>localfiles;
	//得到本地的文件和目录
	DirectoryList(path, localfiles, localdirs);
	localset.insert(localdirs.begin(), localdirs.end());
	localset.insert(localfiles.begin(), localfiles.end());

	//得到数据库中的目录和文件
	std::set<std::string> dbset;
	DataManager::GetInstance()->GetDocs(path, dbset);
	auto localit = localset.begin();
	auto dbit = dbset.begin();

	while (localit != localset.end() && dbit != dbset.end()) {
		//本地数据与数据库中数据一致，跳过
		if (*localit == *dbit) {
			localit++;
			dbit++;
		}
		//本地有，数据库中没有，新增数据
		else if (*localit < *dbit) {
			DataManager::GetInstance()->InsertDocs(path, *localit);
			localit++;
		}
		//本地没有，数据库有，删除数据
		else if (*localit > *dbit) {
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			dbit++;
		}
	}
	//如果没有比较完就直接进行添加或者删除
	while (localit != localset.end()) {
		//新增数据
		DataManager::GetInstance()->InsertDocs(path, *localit);
		localit++;
	}
	while (dbit != dbset.end()) {
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		dbit++;
	}
	//遍历子目录进行数据的导入
	for (const auto& dirs : localdirs) {
		std::string subpath = path;
		subpath += "\\";
		subpath += dirs;
		Scan(subpath);
	}
}

void DocScanManager::MonitorScan(const std::string& path) {
	//通过我们path路径来取得目录下面的数据和数据库中的数据
	std::set<std::string>localset;
	std::vector<std::string>localdirs;
	std::vector<std::string>localfiles;
	//得到本地的文件和目录
	DirectoryList(path, localfiles, localdirs);
	localset.insert(localdirs.begin(), localdirs.end());
	localset.insert(localfiles.begin(), localfiles.end());

	//得到数据库中的目录和文件
	std::set<std::string> dbset;
	DataManager::GetInstance()->GetDocs(path, dbset);
	auto localit = localset.begin();
	auto dbit = dbset.begin();

	while (localit != localset.end() && dbit != dbset.end()) {
		//本地数据与数据库中数据一致，跳过
		if (*localit == *dbit) {
			localit++;
			dbit++;
		}
		//本地有，数据库中没有，新增数据
		else if (*localit < *dbit) {
			DataManager::GetInstance()->InsertDocs(path, *localit);
			localit++;
		}
		//本地没有，数据库有，删除数据
		else if (*localit > *dbit) {
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			dbit++;
		}
	}
	//如果没有比较完就直接进行添加或者删除
	while (localit != localset.end()) {
		//新增数据
		DataManager::GetInstance()->InsertDocs(path, *localit);
		localit++;
	}
	while (dbit != dbset.end()) {
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		dbit++;
	}
}