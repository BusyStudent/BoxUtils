#ifndef _BOXNET_SESSION_HPP_
#define _BOXNET_SESSION_HPP_
#include "socket.hpp"
#include "net_table.hpp"
#include "net_main.hpp"
#include <map>
namespace Box{
	namespace Net{
		class Request;
		class Response;
		class Session{
			//会话
			public:
				//公共变量
				Session();
				~Session();
				Response *send(Request &);//发送请求
				TCPSocket *allocate_socket(const char *ip);//申请一个Socket
				void return_socket(TCPSocket *);//归还Socket
				
				Headers headers;
				bool stream;
			private:
				int max_sockets;//最大Socket数量
				std::map <std::string,TCPSocket*> pool;
				//Socket池子
		};
	};
};
#endif
