#include <string>
#include <list>
#include "exception.hpp"
#include "socket.hpp"
#include "xv.hpp"
using namespace Box;
namespace Box{
namespace Xv{
	//初始化TCP服务器
	TCPServer::TCPServer(Handler &hl,SocketType type,const std::string &ip,uint16_t port)
		:tcp(type),handler(hl){
		//判断类型
		if(type == SocketType::IPV4){
			tcp.bind(AddrV4::From(ip,port));
		}
		else{
			tcp.bind(AddrV6::From(ip,port));
		}
		//监听
		tcp.listen(5);
	}
	TCPServer::~TCPServer(){
		for(auto &client:clients){
			//关闭连接
			handler.close(*client);
			//和Handler说一下
			client->close();
			delete client;
		}
	}
	void TCPServer::run(){
		//运行
		Socket::Set sset;//Socket集合
		int ret;
		while(true){
			sset.clear();//清空
			sset.add(tcp);//添加服务端
			for(auto &client:clients){
				//添加客户
				sset.add(*client);
			}
			ret = Socket::Select(&sset,nullptr,nullptr);
			//进行select
			if(ret == -1){
				//失败了
				return;
			}
			else if(ret != 0){
				//有匹配到的
				if(sset.is_set(tcp)){
					//有请求需要接受
					auto sock = tcp.accept();
					handler.open(*sock);//注册一下
					clients.push_back(sock);
					ret --;
				}
				//匹配一下
			}
		}
	}
};
};
