#ifndef _BOX_XV_HPP_
#define _BOX_XV_HPP_
#include <list>
#include <string>
#include "socket.hpp"
//仿照Python SocketServer写的东西
namespace Box{
	class OSError;
	namespace Xv{
		//设置一些类型的别名
		typedef Socket::BaseSocket BaseSocket;
		typedef Socket::TCP TCP;
		typedef Socket::UDP UDP;
		typedef Socket::AddrV4 AddrV4;
		typedef Socket::AddrV6 AddrV6;
		typedef Socket::Type SocketType;
		class TCPServer;
		class TCPHandler{
			//控制器
			public:
				virtual ~TCPHandler() = 0;
				virtual void open(TCP &sock);//连接被打开的时候
				virtual void close(TCP &sock) = 0;//关闭连接
				virtual void handle(TCP &sock) = 0;//处理数据
				virtual void error(TCP &sock,OSError &err) = 0;//发生错误
				//得到server指针引用
				inline TCPServer &server() const noexcept{
					return *tcp_server;
				};
			private:
				TCPServer *tcp_server;//TCP服务器
			friend class TCPServer;
		};
		class TCPServer{
			//TCP服务器
			public:
				//初始化
				TCPServer(TCPHandler &handler,SocketType,const std::string &ip,uint16_t port);
				~TCPServer();
				void run();//运行
			private:
				Socket::TCP tcp;//监听的Socket
				std::list <TCP*> clients;//客户的Socket
				TCPHandler &handler;
		};
	};
};
#endif
