#pragma once
#include "DocScanManager.h"
#include "DataManager.h"
#include "Monitor.h"
#include <thread>

//初始化扫描
void InitScanManager() {
	DocScanManager::GetInstance()->Scan("C:\\Users\\73117\\Desktop\\leihaoa");
}

//监控文件的实时变化
void MonitorFile() {
	FileWatcher();
}


void RunSearch() {
	while (1) {
		//DataManager s;
		std::vector<std::pair<std::string, std::string>> doc_path;
		doc_path.clear();
		cout << "================================开始查询===================================" << endl << endl;
		std::string key;
		cout << "请输入要查询的关键字：";
		std::cin >> key;
		DataManager::GetInstance()->Search(key, doc_path);

		cout << std::left << std::setw(50) << "名称" << std::left << std::setw(50) << "路径" << endl;
		for (auto& e : doc_path) {
			transform(e.first.begin(), e.first.end(), e.first.begin(), tolower);
			transform(key.begin(),key.end(),key.begin(),tolower);
			std::string prefix, suffix, highlight;
			DataManager::GetInstance()->SetHighLight(e.first, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight);
			cout << suffix;

			int size = prefix.size() + suffix.size() + highlight.size();
			for (int i = 0; i < 50 - size; i++) {
				cout << " ";
			}

			cout << std::left << std::setw(50) << e.second << endl;
		}
		cout << endl;
		cout << "================================查询结束===================================" << endl << endl;
	}
}
 

int main(void) {

	InitScanManager();

	std::thread _t(MonitorFile);//对指定文件目录实时监控
	_t.detach();

	RunSearch();
	system("pause");
	return 0;
}