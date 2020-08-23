#ifndef _BOX_SOCKET_HPP_
#define _BOX_SOCKET_HPP_
//Socket部分
#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2ipdef.h>
	#define BOX_POLL ::WSAPoll
	#define BOX_H_ERRNO ::WSAGetLastError()
#else
	#include <netinet/in.h> 
	#include <poll.h>
	#include <cerrno>
	#define BOX_POLL ::poll
	#define BOX_H_ERRNO h_errno
#endif
#include <ctime>
#include <string>
#include <exception>
#include "libc/attr.h"
namespace Box{
	class OSError;//定义在exception.hpp 
	//基本的Socket
	namespace OS{
		#ifdef _WIN32
			typedef SOCKET NativeSocket;
			typedef WSAPOLLFD NativePollfd;
		#else
			typedef int NativeSocket;//本地的Socket
			typedef pollfd NativePollfd;
		#endif
		//OS::Socket 你要自己手动close
		struct BOXAPI Socket{
			#ifdef _WIN32
				using ssize_t = SSIZE_T;
			#else
				using ssize_t = ::ssize_t;//使用ssize_t
			#endif
			NativeSocket fd;
			operator NativeSocket() const noexcept{
				return fd;
			}
			bool bad() const noexcept{
				//是否无效
				#ifdef _WIN32
				return fd == INVAID_SOCKET;
				#else
				return fd < 0;
				#endif
			}
			//方法 都没有异常
			bool close() noexcept;//关闭
			ssize_t recv(      void *buf,size_t bufsize,int flags = 0) noexcept;
			ssize_t send(const void *buf,size_t bufsize,int flags = 0) noexcept;
			Socket accept(void *addr = nullptr,size_t *addrsize = nullptr) noexcept;
		};
	};
	namespace Net{
		using OS::NativeSocket;
		using OS::NativePollfd;
		enum class SockError{
			//错误代码
			#ifdef _WIN32
			//在非堵塞下 再试一遍
			AGAIN = WSAEWOULDBLOCK,
			WOULDBLOCK = WSAEWOULDBLOCK,
			#else
			AGAIN = EAGAIN,//再试一遍
			WOULDBLOCK = EWOULDBLOCK,
			#endif
		};
		//Socket的错误
		class BOXAPI SocketError:public std::exception{
			public:
				SocketError(int code);
				SocketError(const SocketError &);
				~SocketError();
				int code() const noexcept{
					return errcode; 
				};//错误代码
				const char *what() const noexcept;
				//[[noreturn]] static void Throw(int code);
			private:
				int errcode;
				std::string msg;//信息
		};
		struct BOXAPI AddrV4:public sockaddr_in{
			//IPV4的地址
			AddrV4();
			AddrV4(const std::string &ip,uint16_t port);
			//得到的参数
			//构建从地址中
			static AddrV4 From(const std::string &ip,uint16_t port);
			static AddrV4 FromHost(const std::string &host,uint16_t port);//从地址主机名字
			void set_ip(const std::string &ip) noexcept;//设置IP
			void set_port(uint16_t port) noexcept;//设置端口
			void clear() noexcept;//清空
			std::string get_host() const;//得到主机名字
			std::string get_ip() const;//得到IP
			uint16_t get_port() const;//得到port
			//比较地址
			bool operator ==(const AddrV4 &) const;
			bool operator !=(const AddrV4 &) const;
		};
		struct BOXAPI AddrV6:public sockaddr_in6{
			//IPV6的地址
			AddrV6();
			AddrV6(const std::string &ip,uint16_t port);//从参数构建
			//构建从地址
			static AddrV6 From(const std::string &ip,uint16_t port);
			//操作
			void set_ip(const std::string &ip) noexcept;//设置IP
			void set_port(uint16_t port) noexcept;//设置端口
			void clear() noexcept;//清空
			std::string get_host() const;//得到主机
			std::string get_ip() const;//得到IP
			uint16_t get_port() const;//得到端口
			//比较地址
			bool operator ==(const AddrV6 &) const;
			bool operator !=(const AddrV6 &) const;
		};
		typedef Box::OSError OSError;
		enum class SockFamily{
			//类型
			IPV4 = AF_INET,
			IPV6 = AF_INET6
		};
		class Socket;
		struct BOXAPI SockSet:public fd_set{
			//集合 这里的方法都不会抛出异常
			SockSet();//初始化会被清空
			void clear();//清空
			void add(Socket &sock);//添加一个
			bool is_set(Socket &sock);//被设置了
			#ifndef _WIN32
			//Win32 select不需要这个参数
			NativeSocket max_fd;
			#endif
		};
		class BOXAPI Socket{
			public:
				#ifdef _WIN32
				using ssize_t = SSIZE_T;
				#else
				using ssize_t = ::ssize_t;//使用ssize_t
				#endif
				Socket(const Socket &) = delete;
				Socket(Socket &&);
				Socket(NativeSocket fd);
				Socket(int family,int type,int protocol = 0);
				~Socket();
				bool  is_inherit() const;//是否可继承
				//设置是否可以继承 在子进程
                void set_inherit(bool val = true);
				void set_nonblock(bool var = true);//设置不堵塞
				void close();//关闭流
				void listen(int backlog);//监听
				void bind(const AddrV6 &addr);//绑定IPV6地址
				void bind(const AddrV4 &addr);//绑定IPV4地址
				void bind(const void *addr,size_t addrsize);//操作系统绑定API
				void connect(const AddrV4 &addr);//连接
				void connect(const AddrV6 &addr);//连接IPV6
				void connect(const void *addr,size_t addrsize);//连接API
				//操作系统直接的API 不经过标准缓冲区
				//OS API
				ssize_t send(const void *buf,size_t buflen,int flags = 0) noexcept;//发送
				ssize_t recv(void *buf,size_t buflen,int flags = 0) noexcept;//收数据
				ssize_t sendto(const void *buf,size_t buflen,
							   int flags = 0,const AddrV4 *addr = nullptr) noexcept;
				//发送到一个地址
				ssize_t recvfrom(void *buf,size_t buflen,
								 int flags = 0,AddrV4 *addr = nullptr) noexcept;
				//接受从一个地址
				Socket *accept(AddrV4 *addr = nullptr);//接受客户

				ssize_t operator <<(const std::string &);//写入字符串
				
				AddrV4 get_addrv4_name() const;//返回Socket IPV4的地址 
				AddrV6 get_addrv6_name() const;//返回Socket IPV6的地址 
				
				void get_name(void *addr,size_t addrsize) const;//得到地址通过大小
				template<typename T>
				inline T get_name() const{
					T addr;
					get_name(&addr,sizoef(addr));
					return addr;	
				}

				void get_name(AddrV4 &addr) const;//得到Socket IPV4的名字 相应的地址
				void get_name(AddrV6 &addr) const;//得到Socket IPV6的名字 相应的地址

				//得到与他连接的地址
				template<typename T>
				inline T get_peer_name() const{
					T addr;
					get_peer_name(&addr,sizof(addr));
					return addr;
				}

				void get_peer_name(AddrV4 &addr) const;//得到与他相连的名字 IPV4
				void get_peer_name(AddrV6 &addr) const;//得到与他相连的名字 IPV6
				void get_peer_name(void *addr,size_t addrsize) const;//得到连接的地址名字
				NativeSocket get_fd() const;//得到文件描述符号
				NativeSocket detach_fd();//分离fd
				//复制
				Socket dup();
				//转换为fd
				explicit operator NativeSocket() const noexcept{
					return fd;
				}
				//得到错误和select poll
				static int Poll(NativePollfd [],size_t n,int timeout) noexcept;
				static int Select(SockSet *r_set,SockSet *w_set,SockSet *e_set,const timeval *t = nullptr) noexcept;
				static int GetErrorCode();//得到错误代码
				static std::string GetError();//得到错误
				//创建一对互相连接的Socket
				static void Pair(Socket *[2]);
				//创建操作系统的Socket 
				static NativeSocket Create(int domain,int type,int protocol = 0);
			protected:
				NativeSocket fd;//文件描述符号
			friend struct SockSet;
			friend class Epollfds;
			friend class Pollfds;
			friend class TCP;
			friend class UDP;
		};
		//TCPSocket
		class BOXAPI Tcp:public Socket{
			public:
				using Socket::Socket;
				Tcp(SockFamily family = SockFamily::IPV4);
		};
		//UDPSocket
		class BOXAPI Udp:public Socket{
			public:
				using Socket::Socket;
				Udp(SockFamily family = SockFamily::IPV4);
		};
		BOXAPI [[noreturn]] void throwSocketError(int code = Socket::GetErrorCode());
	};
	//兼容之前写的代码
	typedef Net::Tcp TCPSocket;
	typedef Net::Udp UDPSocket;
};
#endif
