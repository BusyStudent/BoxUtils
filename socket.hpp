#ifndef _BOXUTILS_SOCKET_HPP_
#define _BOXUTILS_SOCKET_HPP_
//Socket部分
#ifdef _WIN32
	#include <winsock.h>
#else
	#include <stdio.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
#endif
#include <string>
namespace BoxUtils{
	//基本的Socket
	class SockAddress;
	class Socket{
		public:
			virtual ~Socket();
			bool bind(SockAddress &);//绑定
			bool close();//关闭
			bool listen(int backlog);//监听
			
			//读取和写
			
			ssize_t write(const void *buf,size_t len);
			ssize_t read(void *buf,size_t len);
			//得到 fd
			int get_fd();
			//通过Error抛出异常
			void throw_for_errno();
			//忽略异常
			void set_noexcept();
			static const char *GetError();
		protected:
			bool noexcept_ = false;
			//默认抛出异常
			int fd;
	};
	class SockAddress{
		public:
			SockAddress();
			SockAddress(struct sockaddr_in &v4_addr);
			SockAddress(struct sockaddr_in6 &v6_addr);
			socklen_t len();//得到长度
			std::string get_ip();
		private:
			union{
				struct sockaddr_in v4;//IPV4
				struct sockaddr_in6 v6;//IPV6
			}addr;
			enum {
				UNKNOWN = 0,
				V4,//IPV4
				V6
			}type;//类型
		friend class Socket;
	};
	class TCPSocket:public Socket{
		public:
			TCPSocket(const TCPSocket &) = delete;
			TCPSocket(bool v4 = true);
			//默认是IPV4的Socket
			TCPSocket *accept();
	};
	class UDPSocket:public Socket{
		public:
			UDPSocket(const UDPSocket &) = delete;
	};
};
#endif
