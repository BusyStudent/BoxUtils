#include <string>
#include <list>
//Linux SIGPIPE信号
#include <csignal>
#include "exception.hpp"
#include "dataguard.hpp"
#include "socket.hpp"
#include "xv.hpp"
using namespace Box;
namespace Box{
namespace Xv{
	//初始化TCP服务器
	TCPServer::TCPServer(TCPHandler &hl,SocketType type,const std::string &ip,uint16_t port)
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
		hl.tcp_server = this;
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
		#ifndef _WIN32
		//忽略SIGPIPE
		DataGuard <void(*)(int)> sig_handler_guard(signal(SIGPIPE,SIG_IGN),
			[](void(* handler)(int sig)) -> void{
				signal(SIGPIPE,handler);//还原handler;
			});
		#endif
		std::list <TCP*> ::iterator iter;
		Socket::Set sset;//Socket集合
		int ret;
		TCP *tcp_socket;//Socket指针
		while(true){
			sset.clear();//清空
			sset.add(tcp);//添加服务端
			//添加客户端
			iter = clients.begin();
			while(iter != clients.end()){
				tcp_socket = *iter;
				if((tcp_socket->get_fd() < 0)){
					//关闭的socket
					delete tcp_socket;
					iter = clients.erase(iter);
				}
				else{
					//添加集合
					sset.add(*tcp_socket);
					++ iter;
				}
			}
			ret = Socket::Select(&sset,nullptr,nullptr);
			//进行select
			if(ret < 0){
				//失败
				return ;
			}
			if(sset.is_set(tcp)){
				//有连接
				try{
					tcp_socket = tcp.accept();
				}
				catch(OSError &err){
					//处理错误
					handler.error(tcp,err);
				}
				ret --;//接入成功
				handler.open(*tcp_socket);
			}
			//迭代clients
			for(auto &tcp_socket:clients){
				if(sset.is_set(*tcp_socket)){
					//有数据可读入
					ret --;
					try{
						//尝试一下
						handler.handle(*tcp_socket);
					}
					catch(OSError &err){
						//有错误
						handler.error(*tcp_socket,err);
					}
				}
				if(ret <= 0){
					//处理完了
					break;
				}
			}
		}
	}
};
};
