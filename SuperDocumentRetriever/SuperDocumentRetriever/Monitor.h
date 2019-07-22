/*=====================================================================================
 监控模块
   1、通过CreateFile获取要监控的目录句柄。
   2、通过ReadDirectoryChangesW来监测到文件系统的变化，还能够返回详细的文件变动的信息，
      并且能够选择是使用同步方式检测还是异步方式监测。
   3、通过Action设置类型过滤器，根据过滤器的设置，ReadDirectoryChangesW函数可以监控文
      件名改变、文件属性改变、文件大小改变、文件内容被改写、文件被删除等多种类型的变化。
=======================================================================================*/


#pragma  once
#include "Common.h"
#include <tchar.h>


void get_dir_path(std::string& s) {
	int pos = -1;
	for (int i = s.size() - 1; i >= 0; i--) {
		if (s[i] == '\\') {
			pos = i;
			break;
		}
	}
	if (pos != -1) {
		s = s.substr(0, pos);
	}
}

void FileWatcher()
{
	DWORD cbBytes;
	char file_name[MAX_PATH] = { '\0' }; //设置文件名;
	char file_rename[MAX_PATH] = { '\0' }; //设置文件重命名后的名字;
	char notify[1024] = { '\0' };
	int count = 0; //文件数量。可能同时拷贝、删除多个文件，可以进行更友好的提示;
	TCHAR *dir = (TCHAR*)_T("C:\\Users\\73117\\Desktop\\leihaoa");
	std::string s = "C:\\Users\\73117\\Desktop\\leihaoa";
	//HANDLE就是一个句柄
	HANDLE dirHandle = CreateFile(dir,
		GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	if (dirHandle == INVALID_HANDLE_VALUE) { //若网络重定向或目标文件系统不支持该操作，函数失败，同时调用GetLastError()返回ERROR_INVALID_FUNCTION
		cout << "error" + GetLastError() << endl;
	}

	//FILE_NOTIFY_INFORMATION是一个结构体，是柔型数组,将数组强转为结构体指针
	memset(notify, 0, strlen(notify));
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify;

	while (true) {
		if (ReadDirectoryChangesW(dirHandle, &notify, 1024, true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
			&cbBytes, NULL, NULL))
		{
			pnotify = (FILE_NOTIFY_INFORMATION*)notify;
			//转换文件名为多字节字符串;
			if (pnotify->FileName) {
				memset(file_name, 0, strlen(file_name));
				int ret = WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, pnotify->FileNameLength / 2, file_name, 99, NULL, NULL);
				if (ret == 0) {
					GetLastError();
				}
			}
			if ((pnotify->Action == FILE_ACTION_ADDED) | (pnotify->Action == FILE_ACTION_REMOVED) | (pnotify->Action == FILE_ACTION_RENAMED_OLD_NAME)) {

				std::string dirpath = s + "\\" + file_name;
				get_dir_path(dirpath);
				//	cout << dirpath << endl;
					//返回上一层的目录
				DocScanManager::GetInstance()->MonitorScan(dirpath);
				//cout << "扫描模块的地址monitor" << ScanManager::CreateIntance() << endl;
				//cout << "DirPath:" << dirpath << endl;
			}
		}
	}
	CloseHandle(dirHandle);
}
