#ifndef _BOX_XV_HPP_
#define _BOX_XV_HPP_
#include <vector>
#include "socket.hpp"
namespace Box{
	namespace Xv{
		class Server{
			public:
				Server();
				virtual ~Server();
				bool bind(const char *ip,uint16_t port);
				bool listen(int backlog = 5);
				void run();//启动
				void clear();//清空
				virtual bool recv_data(TCPSocket *) = 0;//读取数据
				virtual bool accept_user(TCPSocket *,SockAddress &addr);//接入用户
			private:
				void (*HandleErr)(Server &,TCPSocket *) = nullptr;//处理错误的Socket
				
				std::vector <TCPSocket*> socks_vec;//连接池
				TCPSocket *sock;//套接字
		};
		class Client{
			
		};
	};
};
#endif
