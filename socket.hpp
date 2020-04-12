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
	//基本的Socket
	namespace Socket{
		#ifdef _WIN32
			typedef SOCKET NativeSocket;
		#else
			typedef int NativeSocket;//本地的Socket
		#endif
		enum class ErrorCode{
			AGAIN = EAGAIN,//再试一遍
		};
		struct ReadError{
			//读错误
			size_t read_size;//读入大小
			ErrorCode code;//错误代码
			const char *msg;
		};
		struct WriteError{
			//写错误
			ssize_t write_size;
			ErrorCode code;
			const char *msg;
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
		};
		struct OSError{
			//操作系统的错误
			int code;
			const char *msg;
		};
		enum class Type{
			//类型
			IPV4,
			IPV6
		};
		class BaseSocket{
			public:
				BaseSocket(NativeSocket fd);
				~BaseSocket();
				void set_nonblock(bool var);//设置不堵塞
				void close();//关闭流
				void flush();//刷新输出流
				void listen(int backlog);//监听
				void bind(const AddrV6 &addr);//绑定IPV6地址
				void bind(const AddrV4 &addr);//绑定IPV4地址
				void connect(const AddrV4 &addr);//连接
				void connect(const AddrV6 &addr);//连接IPV6
				//C函数标准库的封装
				int printf(const char *fmt,...) noexcept;//打印输出
				int scanf(const char *fmt,...) noexcept;//扫描
				char *gets(char *buf,int bufsize) noexcept;//读入一行
				int puts(const char *str) noexcept;//输出一行
				int getc() noexcept;//得到一个字符
				int putc(int ch) noexcept;//放置一个字符
				int ungetc(int ch) noexcept;//放回一个字符
				//操作系统直接的API 不经过标准缓冲区
				ssize_t sendto(const void *buf,size_t buflen,
							   int flags = 0,const AddrV4 *addr = nullptr) noexcept;
				//发送到一个地址
				ssize_t recvfrom(void *buf,size_t buflen,
								 int flags = 0,AddrV4 *addr = nullptr) noexcept;
				//接受从一个地址
				//C标准库的封装
				size_t read(void *buf,size_t size);//读入
				size_t write(const void *buf,size_t size);//写
				
				size_t operator <<(const std::string &);//写入字符串
				size_t operator >>(std::string &);//读入一行字符串
				
				AddrV4 get_addrv4_name() const;//返回Socket IPV4的地址 
				AddrV6 get_addrv6_name() const;//返回Socket IPV6的地址 
				void get_name(AddrV4 &addr) const;//得到Socket IPV4的名字 相应的地址
				void get_name(AddrV6 &addr) const;//得到Socket IPV6的名字 相应的地址
				void get_peer_name(AddrV4 &addr) const;//得到与他相连的名字 IPV4
				void get_peer_name(AddrV6 &addr) const;//得到与他相连的名字 IPV6
				NativeSocket get_fd() const;//得到文件描述符号
				FILE *in() const;//得到输如流
				FILE *out() const;//得到输出流
			protected:
				BaseSocket();
				BaseSocket(const BaseSocket &) = delete;
				void set_fd(NativeSocket fd);//设置fd
				
				NativeSocket fd;//文件描述符号
				FILE *fin;//输入流
				FILE *fout;//输出流
			friend struct Set;
			friend class TCP;
			friend class UDP;
			friend void CreatePair(BaseSocket**,BaseSocket**);
		};
		//TCPSocket
		class TCP:public BaseSocket{
			public:
				TCP(Type type = Type::IPV4);
				//OS API
				ssize_t send(const void *buf,size_t buflen,int flags = 0) noexcept;//发送
				ssize_t recv(void *buf,size_t buflen,int flags = 0) noexcept;//收数据
				
				TCP *accept(AddrV4 *addr = nullptr);//接受客户
				//这个在Window有自己的实现
		};
		//UDPSocket
		class UDP:public BaseSocket{
			public:
				UDP(Type type = Type::IPV4);
		};
		struct Set:public fd_set{
			//集合 这里的方法都不会抛出异常
			Set();//初始化会被清空
			void clear();//清空
			void add(BaseSocket &sock);//添加一个
			bool is_set(BaseSocket &sock);//被设置了
			#ifndef _WIN32
			//Win32 select不需要这个参数
			NativeSocket max_fd;
			#endif
		};
		//创建一对互相连接的Socket
		void CreatePair(BaseSocket **s1,BaseSocket **s2);
		int Select(Set *r_set,Set *w_set,Set *e_set,const timeval *t = nullptr);
		int GetErrorCode();//得到错误代码
		const char *GetError();//得到错误
	};
	//兼容之前写的代码
	typedef Socket::TCP TCPSocket;
	typedef Socket::UDP UDPSocket;
};
#endif
