#include "httplib.h"
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <boost/filesystem.hpp>

namespace bf = boost::filesystem;

#define LIST_PATH "./list/"


using namespace httplib;


//创建一个服务端类
class P2PServer{
  public:

    //开始运行
    void Strart(){
      if(!bf::exists(LIST_PATH)){
        bf::create_directory(LIST_PATH);
      }
      _server.Get("/hostpair",GetHostPair);
      _server.Get("/list",GetFileList);
      _server.Get("/list/(.*)",GetFileDownload);
      _server.listen("192.168.145.158",9000);
    }

  private:
    //提供客户端主机配对请求响应
    static void GetHostPair(const Request& req,Response& rsp){
      std::cout<<"服务端收到了一条 "<<req.method<<" 请求" << std::endl;
      rsp.status = 200;
    }
    //文件列表请求处理
    static void GetFileList(const Request& req,Response& res){
      std::cout<<"服务端收到了一条 "<<req.method<<" 请求" << std::endl;
      std::string realpath = LIST_PATH;
      if(!bf::exists(realpath)){
        //如果目录不存在的话就直接创建一个新的realpath文件
        bf::create_directory(realpath);
      }
      std::vector<std::string> list;
      bf::directory_iterator it_begin(realpath);
      bf::directory_iterator it_end;
      //通过迭代器去访问目录下面的文件
      for(;it_begin != it_end; it_begin++){
        if(bf::is_directory(it_begin->status())){
          continue;
        }
        //不是目录的时候将文件添加到list中去
        std::string name = it_begin->path().filename().string();
        list.push_back(name); 
      }
      std::string body;
      for(auto& e:list){
        std::cout<<e<<std::endl;
        body += e + "\n";
      }
      //将数据发送过去
      std::cout<<body<<std::endl;
      res.body = body;
      res.set_header("Content-Type","text/html");
      res.set_header("Content-Length",std::to_string(body.size()).c_str());
      res.status = 200;
      return;
    }

    //分块下载
    static void GetRange(std::string& range, int64_t& range_start, int64_t& range_end) {
      //Range:bytes=begin-end
      //找 = 的位置
      int pos1 = range.find("=");
      //找到-的位置
      int pos2 = range.find("-");                                       
      range_start = atoi(range.substr(pos1+1,pos2-pos1-1).c_str());
      range_end = atoi(range.substr(pos2+1).c_str());
    }
    /*
    //提供客户端下载文件响应
    static void GetFileDownload(const Request& req,Response rsp){
      std::string realpath = "." + req.path; 
      if(!bf::exists(realpath)){
        rsp.status = 404;
        return ;
      }
      //获取文件大小
      int64_t fsize = bf::file_size(realpath);
      std::cout << "文件总大小为" << fsize << std::endl;
      if (req.method == "HEAD") {
        rsp.set_header("Content-Length",std::to_string(fsize).c_str());
        uint64_t fsize = atoi(rsp.get_header_value("Content-Length").c_str());                                                             
        //长度
        std::cout<<"文件长度fsize:"<<fsize<<std::endl;
        std::cout<<"服务端收到了一条 "<<req.method<<" 请求" << std::endl;
        rsp.status = 200;
        return ;
      }
      //如果是Get请求就要分块下载文件
      std::ifstream file(realpath,std::ios::binary);

      if(!file.is_open()){
        rsp.status = 500;
        return ;
      }


      int64_t range_start=0;
      int64_t range_end=0;
      std::string range;
      range = req.get_header_value("Range");
      std::cout << range << std::endl;
      GetRange(range, range_start, range_end);
      std::cout<<"begin:"<<range_start <<"\tend:"<<range_end<<std::endl;
      int64_t  range_size = range_end - range_start + 1;
      std::cout<<"分块大小"<< range_size <<std::endl;
      rsp.body.resize(range_size);
      file.seekg(range_start,std::ios::beg);
      file.read(&rsp.body[0],range_size);                                                                                                                     
      //判断此次文件操作是成功
      if(!file.good()){
        rsp.status = 500;
        return;
      }
      file.close();
      std::cout<<"服务端收到了一条 "<<req.method<<" 请求" << std::endl;
      rsp.set_header("Content-Type","application/octet-stream");
      rsp.status = 200;
      return ;
    }
    */
    static void GetFileDownload(const Request& req, Response& rsp) {
      std::string realpath = "." + req.path;
      if (!bf::exists(realpath)) {
        //文件不存在
        rsp.status = 404;
        return;
      }
      //获取文件的大小
      int64_t fsize = bf::file_size(realpath);
      std::cout << "文件总大小：" << fsize << std::endl;
      if (req.method == "HEAD") {
        rsp.set_header("Content-Length", std::to_string(fsize).c_str());
        std::cout<<"服务端收到了一条 "<<req.method<<" 请求" << std::endl;
        rsp.status = 200;
        return;
      }
      //如果是GET请求，则要下载文件
      std::ifstream file(realpath, std::ios::binary);
      if (!file.is_open()) {
        //文件打开失败
        rsp.status = 500;
        return;

      }

      //Range:bytes=start-end;
      int64_t range_start = 0;
      int64_t range_end = 0;
      std::string range;
      //获取分块内容
      range = req.get_header_value("Range");
      std::cout << range << std::endl;
      
      GetRange(range, range_start, range_end);
      std::cout << "range_start:" << range_start << "\trange_end:" << range_end << std::endl;
      int64_t  range_size = range_end - range_start + 1;
      std::cout << "分块大小" << range_size << std::endl;
      rsp.body.resize(range_size);
      file.seekg(range_start, std::ios::beg);
      file.read(&rsp.body[0], range_size);
      //判断文件操作是否成功
      if (!file.good()) {
        rsp.status = 500;
        return;

      }
      file.close();
      //传输body,设置application/octet-stream表示就是下载文件
      rsp.set_header("Content-Type", "application/octet-stream");
      rsp.status = 200;

      return;

    }


  private:
    httplib::Server _server;
};



int main(void){

  P2PServer srv;
  srv.Strart();

  return 0;
}
