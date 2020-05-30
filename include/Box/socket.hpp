#ifndef _BOX_SOCKET_HPP_
#define _BOX_SOCKET_HPP_
//Socket部分
#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2ipdef.h>
#else
	#include <netinet/in.h> 
#endif
#include <ctime>
#include <cstdio>
#include <string>
namespace Box{
	class OSError;//定义在exception.hpp 
	//基本的Socket
	namespace Net{
		#ifdef _WIN32
			typedef SOCKET NativeSocket;
		#else
			typedef int NativeSocket;//本地的Socket
		#endif
		enum class ErrorCode{
			AGAIN = EAGAIN,//再试一遍
		};
		struct AddrV4:public sockaddr_in{
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
		struct AddrV6:public sockaddr_in6{
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
		struct SockSet:public fd_set{
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
		class Socket{
			public:
				Socket(const Socket &) = delete;
				Socket(Socket &&);
				Socket(NativeSocket fd);
				Socket(int family,int type,int protocol = 0);
				~Socket();
				void set_nonblock(bool var);//设置不堵塞
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
				//得到错误和select
				static int Select(SockSet *r_set,SockSet *w_set,SockSet *e_set,const timeval *t = nullptr);
				static int GetErrorCode();//得到错误代码
				static const char *GetError();//得到错误
				//创建一对互相连接的Socket
				static void Pair(Socket *[2]);
				//创建操作系统的Socket 
				static NativeSocket Create(int domain,int type,int protocol = 0);
			protected:
				NativeSocket fd;//文件描述符号
			friend struct SockSet;
			friend class TCP;
			friend class UDP;
		};
		//TCPSocket
		class TCP:public Socket{
			public:
				using Socket::Socket;
				TCP(SockFamily family = SockFamily::IPV4);
		};
		//UDPSocket
		class UDP:public Socket{
			public:
				using Socket::Socket;
				UDP(SockFamily family = SockFamily::IPV4);
		};
	};
	//兼容之前写的代码
	typedef Net::TCP TCPSocket;
	typedef Net::UDP UDPSocket;
};
#endif
