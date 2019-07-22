#include "DocScanManager.h"

void DocScanManager::Scan(const std::string& path) {
	//ͨ������path·����ȡ��Ŀ¼��������ݺ����ݿ��е�����
	std::set<std::string>localset;
	std::vector<std::string>localdirs;
	std::vector<std::string>localfiles;
	//�õ����ص��ļ���Ŀ¼
	DirectoryList(path, localfiles, localdirs);
	localset.insert(localdirs.begin(), localdirs.end());
	localset.insert(localfiles.begin(), localfiles.end());

	//�õ����ݿ��е�Ŀ¼���ļ�
	std::set<std::string> dbset;
	DataManager::GetInstance()->GetDocs(path, dbset);
	auto localit = localset.begin();
	auto dbit = dbset.begin();

	while (localit != localset.end() && dbit != dbset.end()) {
		//�������������ݿ�������һ�£�����
		if (*localit == *dbit) {
			localit++;
			dbit++;
		}
		//�����У����ݿ���û�У���������
		else if (*localit < *dbit) {
			DataManager::GetInstance()->InsertDocs(path, *localit);
			localit++;
		}
		//����û�У����ݿ��У�ɾ������
		else if (*localit > *dbit) {
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			dbit++;
		}
	}
	//���û�бȽ����ֱ�ӽ�����ӻ���ɾ��
	while (localit != localset.end()) {
		//��������
		DataManager::GetInstance()->InsertDocs(path, *localit);
		localit++;
	}
	while (dbit != dbset.end()) {
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		dbit++;
	}
	//������Ŀ¼�������ݵĵ���
	for (const auto& dirs : localdirs) {
		std::string subpath = path;
		subpath += "\\";
		subpath += dirs;
		Scan(subpath);
	}
}

void DocScanManager::MonitorScan(const std::string& path) {
	//ͨ������path·����ȡ��Ŀ¼��������ݺ����ݿ��е�����
	std::set<std::string>localset;
	std::vector<std::string>localdirs;
	std::vector<std::string>localfiles;
	//�õ����ص��ļ���Ŀ¼
	DirectoryList(path, localfiles, localdirs);
	localset.insert(localdirs.begin(), localdirs.end());
	localset.insert(localfiles.begin(), localfiles.end());

	//�õ����ݿ��е�Ŀ¼���ļ�
	std::set<std::string> dbset;
	DataManager::GetInstance()->GetDocs(path, dbset);
	auto localit = localset.begin();
	auto dbit = dbset.begin();

	while (localit != localset.end() && dbit != dbset.end()) {
		//�������������ݿ�������һ�£�����
		if (*localit == *dbit) {
			localit++;
			dbit++;
		}
		//�����У����ݿ���û�У���������
		else if (*localit < *dbit) {
			DataManager::GetInstance()->InsertDocs(path, *localit);
			localit++;
		}
		//����û�У����ݿ��У�ɾ������
		else if (*localit > *dbit) {
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			dbit++;
		}
	}
	//���û�бȽ����ֱ�ӽ�����ӻ���ɾ��
	while (localit != localset.end()) {
		//��������
		DataManager::GetInstance()->InsertDocs(path, *localit);
		localit++;
	}
	while (dbit != dbset.end()) {
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		dbit++;
	}
}