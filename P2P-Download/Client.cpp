#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <ifaddrs.h>
#include "httplib.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <thread>
#include <boost/thread/thread.hpp>


namespace bf = boost::filesystem;
const std::string  DOWNPATH =  "./download";
const int  MAXFILE = 100*1024*1024;

class P2Pclient{
  private:

    //获取局域网内所有的主机序列
    void GetHostpath(){
      //通过getifaddr()来获取主机的ip和地址
      struct ifaddrs* addrs = NULL;
      getifaddrs(&addrs);
      while(addrs){
        if(std::string(addrs->ifa_name) == "lo"){
          addrs = addrs->ifa_next;
          continue;
        }
        //获取当前的ip地址和掩码
        struct sockaddr_in* addr = (struct sockaddr_in*)addrs->ifa_addr;
        struct sockaddr_in* mask = (struct sockaddr_in*)addrs->ifa_netmask;
        //如果不是IPV4的话就直接略过
        if(addr->sin_family != AF_INET){
          addrs = addrs->ifa_next;
          continue;
        }
        if(addr->sin_addr.s_addr == inet_addr("192.168.122.1")){
          addrs = addrs->ifa_next;
          continue;
        }
        //此时获取出网络号和掩码并且转化为本机字节序
        uint32_t ipaddr = ntohl(addr->sin_addr.s_addr & \
            mask->sin_addr.s_addr);
        //最大主机数
        uint32_t maxhost = ntohl(~(mask->sin_addr.s_addr));
        //将数据添加到hostpath中去
        for(uint32_t i = 2;i < maxhost-1;i++){
            struct in_addr ip;
            ip.s_addr = htonl(ipaddr+i);
            //将字节序转换为字符串
            // std::cerr <<inet_ntoa(ip)<<"在范围中\n";
            _hostpath.push_back(inet_ntoa(ip));
        }
        addrs = addrs->ifa_next;
      }
      return;
    }


    void Thr_Online(int i){

        httplib::Client client(_hostpath[i].c_str(),9000);
        auto res = client.Get("/hostpair");
        if(res && res->status == 200){
          //服务端返回200状态码此时成功
        std::cerr << _hostpath[i] << ":匹配成功\n";
          _onlinepath.push_back(_hostpath[i]);
        }
        else{
          printf("%s:匹配失败\n",_hostpath[i].c_str());
        }
    }
  
    //获取局域网内当前在线主机,采用多线程压缩配对等待时间
    void GetOnlinepath(){
      std::vector<std::thread> vthr;//存储线程，方便join
      vthr.resize(_hostpath.size()-3);
      int i = 0 , j = 0;
      for(i = 2;i < (int)_hostpath.size()-1; i++){
        vthr[j++] = std::thread(&P2Pclient::Thr_Online, this, i);
      }
      for(size_t i = 0;i < vthr.size(); i++){
        vthr[i].join();
      }
      //打印在线的主机序列
      std::cout << "当前在线的主机序列如下：\n";
      for(i = 0; i < _onlinepath.size(); i++){
        std::cout<<"[" << i <<"]" <<_onlinepath[i]<<std::endl;
      }
      //选择特定的ip地址向服务端发起请求，得到共享文件目录下面的文件夹
      std::cout<<"请输入你想查看的主机序号:";
      std::flush(std::cout);
      std::cin >> _onlineindex;  
      return;
    }

    void GetFileList(){
      //ip地址
      std::string ip_addr = _onlinepath[_onlineindex];
      //现在将向服务端发起请求，得到我们的数据
      httplib::Client client(ip_addr.c_str(),9000);
      //存储我们获得的共享目录下的文件，
      std::vector<std::string> file_list;
      auto res = client.Get("/list");
    
      if(res && res->status == 200){
        //此时得到数据成功，将数据使用boost库中库sqlit来切割
        std::string body = res->body;
        //使用boost库中的split(file_list,sec_string,is_any_of(""))
        boost::split(file_list,body,boost::is_any_of("\n"));

      }else{
        std::cerr << "获取文件失败\n";
        return;
      }
      for(size_t i = 0;i < file_list.size(); i++){
        if(file_list[i].empty()){
          continue;
        }
        std::cout<<"[" << i <<"]" << file_list[i] << std::endl;
        _file_list.push_back(file_list[i]);
      }
      return;
    }
  
    //线程入口函数
    void thr_begin(std::string path, uint64_t begin,uint64_t end, int *flag){
      
      //调试信息<
      std::cout << "range_start: "<< begin << "\trange_end:"<< end <<std::endl; 

      *flag = 0;
		  std::string downfile = DOWNPATH + "/"  +path; 
     //先得到发送的主机ip
      std::string host = _onlinepath[_onlineindex];
      //path是文件选择文件的名称
      //拼接上Range的头信息，
      std::string range = "bytes=" + std::to_string(begin) + "-" + std::to_string(end);
      std::string realpath = "/list/"+path;
      httplib::Headers head;
      head.insert(std::make_pair("Range",range.c_str()));

      //向服务端发送请求
      httplib::Client client(host.c_str(),9000);
      auto res = client.Get(realpath.c_str() , head);
      if(res && res->status == 200){
        //打开文件并且文件不存在的时候创建新文件
        if(!bf::exists(DOWNPATH)){
          //此时新建存储下载文件目录
          bf::create_directory(DOWNPATH);
        }
        //使用文件流写入必须文件存在。所以要创建一个名字相同的文件名
        int fd = open(downfile.c_str(),O_CREAT,0664);
        if(fd < 0){
          std::cerr << "文件创建失败\n";
        }
        close(fd);
        std::fstream file(downfile, std::ios::binary | std::ios::out | std::ios::in);
        if(!file.is_open()){
          std::cerr << "文件流打开失败\n";
          return;
        }
        //设置偏移量
        file.seekp(begin,std::ios::beg);
        file.write(&res->body[0], res->body.size());
        if(!file.good()){
          std::cerr << "文件流出错了\n";
          return;
        }
        file.close();
        *flag = 1;
        std::cerr << "分块下载成功\n";
        return;
      } 
      std::cerr << "分块下载出错了\n";
      return;
    }

    void GetDownload(){
      std::cout << "目前连接的ip:" << _onlinepath[_onlineindex]<<std::endl;
      std::cout<<"主机文件列表：" << std::endl;
      for(size_t i = 0;i < _file_list.size(); i++){
        std::cout<<"["<< i << "]:" << _file_list[i] << std::endl;
      }
      std::cout<< "选择主机文件的编号：";
      std::fflush(stdout);
      int file_index = 0;
      std::cin >> file_index;
      std::string real_path = "/list/";
      httplib::Client client(_onlinepath[_onlineindex].c_str(),9000);
      real_path = real_path + _file_list[file_index];
      //先发送一个Head
      auto res = client.Head(real_path.c_str());
      if(!(res && res->status == 200)){
        std::cerr << "请求错误" << std::endl;
        return;
      }
      uint64_t fsize = atoi(res->get_header_value("Content-Length").c_str());
      //长度
      std::cout<<"文件总大小fsize:"<<fsize<<std::endl;

      uint64_t start = 0;
      uint64_t end = 0;
      int size = fsize / MAXFILE;
      std::vector<boost::thread>threadv;
      threadv.resize(size + 1);
      std::vector<int> flag;
      flag.resize(size + 1,1);
 	    std::string path = _file_list[file_index];     
      //Range:bytes=begin-ends;
      for(uint64_t i = 0;i <= size; i++){
        start = i * MAXFILE;
        end = (i + 1) * MAXFILE - 1;       
        if(i == size){
          //表示,最后一个，如果能够求模最后一个位置不能为
          end = fsize - 1;
        }        
        //创建一个线程去实现传输。并且传递进path是文件的名称
        threadv[i] = (boost::thread(&P2Pclient::thr_begin, this, path, start, end, &flag[i]));
      }
      for(size_t i = 0;i < threadv.size(); i++){
        threadv[i].join();
      }
      //此时判断所有的文件是不是都
      //下载文件失败
      for(int i = 0; i < size;i++){
        if(flag[i] == 0){
          std::cerr  << "下载失败\n";
          return;
        }
      }
      std::cerr << "下载文件成功\n";
      return;
    }

  public:
    void Run(){
      int num = 0;
      std::cout << "=======================================\n";
      std::cout << "1.搜索在线在主机,选择指定主机\n";
      std::cout << "2.查看指定主机下的共享文件\n";
      std::cout << "3.选择对应文件进行下载\n";
      std::cout << "0.退出系统\n";
      std::cout << "=======================================\n";
      std::cout << "请输入您的选择：";
      std::cin >> num;
      switch(num){
        case 1:
          GetHostpath(); 
          GetOnlinepath();
          break;
        case 2:
          GetFileList();
          break;
        case 3:
          GetDownload();
          break;
        case 0:
          std::cout << "Bye!\n";
          exit(0);
        default:
          std::cout << "操作失败，请重新选择";
          break;
      }
      
      std::cout<<std::endl<<std::endl;
    }
  private:
    //附近主机列表
    std::vector<std::string> _hostpath;
    //在线主机列表
    std::vector<std::string> _onlinepath;
    //在线主机选择下标
    int _onlineindex;
    //文件列表
    std::vector<std::string> _file_list;
};

int main(){

  P2Pclient client;
  while(1){
    client.Run();
  }
  return 0;
}
