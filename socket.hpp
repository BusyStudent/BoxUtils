#ifndef _BOX_SOCKET_HPP_
#define _BOX_SOCKET_HPP_
//Socket部分
#ifdef _WIN32
	#include <winsock.h>
	#include <io.h>
	#include <process.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>  
	#include <netinet/in.h> 
#endif
#include <string>
namespace Box{
	//基本的Socket
	namespace SocketType{
		//socket类型
		enum SocketType{
			IPV4 = true,
			IPV6 = false
		};
	};
	class SockAddress;
	class SocketSet;
	class Socket{
		public:
			virtual ~Socket();
			bool bind(const char *ip,uint16_t port);
			bool bind(SockAddress &);//绑定
			bool close();//关闭
			bool listen(int backlog);//监听
			bool set_flags();
			void set_noblock();//设置非阻塞
			void set_block();//设置阻塞
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
			static int Select(SocketSet *r,SocketSet *w,SocketSet *e,struct timeval *t);
			//Select套接字
		protected:
			bool noexcept_ = false;
			//默认抛出异常
			int fd;
		friend class Xv;
		friend class SocketSet;
	};
	class SocketSet{
		//Socket集合
		public:
			SocketSet();
			void add(Socket *);//加入一个
			//移除要自己清空 以为如果fd会发生变化
			void clear();
			bool is_set(Socket *);//被设置了
		private:
			int maxfd;
			fd_set _set;
		friend class Socket;
	};
	class SockAddress{
		public:
			SockAddress();
			SockAddress(const char *ip,uint16_t port);
			SockAddress(const struct sockaddr_in &v4_addr);
			SockAddress(const struct sockaddr_in6 &v6_addr);
			socklen_t len();//得到长度
			std::string get_ip();
			uint16_t get_port();//得到端口
			
			static void LoadStruct(SockAddress&,const struct sockaddr_in &);//加载结构体
			static void LoadStruct(SockAddress&,const struct sockaddr_in6&);
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
		friend class TCPSocket;
		friend class UDPSocket;
	};
	class TCPSocket:public Socket{
		public:
			TCPSocket(const TCPSocket &) = delete;
			TCPSocket(bool v4 = true);
			//默认是IPV4的Socket
			TCPSocket *accept(SockAddress *addr = nullptr);
			//等待接入
		private:
			bool v4;
	};
	class UDPSocket:public Socket{
		public:
			UDPSocket(bool v4 = true);
			UDPSocket(const UDPSocket &) = delete;
			//UDP特有的sendto和recvfrom
			ssize_t sendto(const void *buf,size_t len,SockAddress &addr);//发送信息
			ssize_t sendto(const void *buf,size_t len,const char *ip,uint16_t port);//发送信息
			ssize_t recvfrom(void *buf,size_t len,SockAddress *addr = nullptr);//接收信息
			ssize_t recvfrom(void *buf,size_t len,const char **ip = nullptr,uint16_t *port = nullptr);//接收信息
		private:
			bool v4;//是否是IPV4 
			//要么就IPV6
	};
};
#endif
