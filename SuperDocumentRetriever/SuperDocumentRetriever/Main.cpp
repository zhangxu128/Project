#pragma once
#include "DocScanManager.h"
#include "DataManager.h"
#include "Monitor.h"
#include <thread>

//��ʼ��ɨ��
void InitScanManager() {
	DocScanManager::GetInstance()->Scan("C:\\Users\\73117\\Desktop\\leihaoa");
}

//����ļ���ʵʱ�仯
void MonitorFile() {
	FileWatcher();
}


void RunSearch() {
	while (1) {
		//DataManager s;
		std::vector<std::pair<std::string, std::string>> doc_path;
		doc_path.clear();
		cout << "================================��ʼ��ѯ===================================" << endl << endl;
		std::string key;
		cout << "������Ҫ��ѯ�Ĺؼ��֣�";
		std::cin >> key;
		DataManager::GetInstance()->Search(key, doc_path);

		cout << std::left << std::setw(50) << "����" << std::left << std::setw(50) << "·��" << endl;
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
		cout << "================================��ѯ����===================================" << endl << endl;
	}
}
 

int main(void) {

	InitScanManager();

	std::thread _t(MonitorFile);//��ָ���ļ�Ŀ¼ʵʱ���
	_t.detach();

	RunSearch();
	system("pause");
	return 0;
}