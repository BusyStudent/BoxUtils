#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include "backtrace.hpp"
#include "exception.hpp"
#include "socket.hpp"
#ifdef _WIN32
	#define fdopen _fdopen
	//fdopen的宏
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#include <io.h>
	#include <process.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netinet/in.h> 
	#include <netdb.h>
#endif
using namespace Box::Socket;
using namespace Box;
namespace{
	void throw_os_error(){
		//抛出OS错误
		throw Box::OSError(errno);
	};
	void throw_os_netdb_error(){
		//抛出netdb里面函数错误
		throw Box::OSError(h_errno,hstrerror(h_errno));
	};
	inline void sock_bind(NativeSocket fd,const AddrV4 &addr){
		if(bind(fd,(const sockaddr*)&addr,sizeof(sockaddr_in)) != 0){
			//失败
			throw_os_error();
		}
	};
	inline void sock_bind(NativeSocket fd,const AddrV6 &addr){
		//IPV6版本绑定
		if(bind(fd,(const sockaddr*)&addr,sizeof(sockaddr_in6)) != 0){
			throw_os_error();
		}
	};
	inline void sock_connect(NativeSocket fd,const AddrV4 &addr){
		if(connect(fd,(const sockaddr*)&addr,sizeof(sockaddr_in)) != 0){
			//失败
			throw_os_error();
		}
	};//连接
	//IPV6
	inline void sock_connect(NativeSocket fd,const AddrV6 &addr){
		if(connect(fd,(const sockaddr*)&addr,sizeof(sockaddr_in6)) != 0){
			throw_os_error();
		}
	}
	inline void sock_listen(NativeSocket fd,int backlog){
		//监听
		if(listen(fd,backlog) != 0){
			throw_os_error();
		}
	};
	inline int sock_accept(NativeSocket fd,AddrV4 *addr){
		//接受客户 addr是客户的地址
		#ifdef _WIN32
		int s = sizeof(sockaddr_in);
		#else
		socklen_t s = sizeof(sockaddr_in);
		#endif
		NativeSocket cfd = accept(fd,(sockaddr*)addr,&s);
		if(cfd < 0){
			//失败
			throw_os_error();
		}
		return cfd;
	};
	inline int f_ungetc(int ch,FILE *stream){
		return ungetc(ch,stream);
	};
	//send recv
	inline ssize_t sock_recv(NativeSocket fd,void *buf,size_t buflen,int flags){
		#ifdef _WIN32
		//WinSock
			return recv(fd,(char*)buf,buflen,flags);
		#else
			return recv(fd,buf,buflen,flags);
		#endif
	};
	inline ssize_t sock_send(NativeSocket fd,const void *buf,size_t buflen,int flags){
		#ifdef _WIN32
		//WinSock
			return send(fd,(const char*)buf,buflen,flags);
		#else
			return send(fd,buf,buflen,flags);
		#endif
	};
	//sendto recvfrom
	inline ssize_t sock_sendto(NativeSocket fd,const void *buf,size_t buflen,int flags,const AddrV4 *addr){
		#ifdef _WIN32
			return sendto(fd,(const char*)buf,buflen,flags,(const sockaddr*)addr,sizeof(sockaddr_in));
		#else
			return sendto(fd,buf,buflen,flags,(const sockaddr*)addr,sizeof(sockaddr_in));
		#endif
	};
	inline ssize_t sock_recvfrom(NativeSocket fd,void *buf,size_t len,int flags,AddrV4 *addr){
		#ifdef _WIN32
			int s = sizeof(sockaddr_in);
			return recvfrom(fd,(char*)buf,len,flags,(sockaddr*)addr,&s);
		#else
			socklen_t s = sizeof(sockaddr_in);
			return recvfrom(fd,buf,len,flags,(sockaddr*)addr,&s);
		#endif
	};
	//得到主机的信息通过Addr
	inline hostent *os_gethostbyaddr(const AddrV4 *addr){
		#ifdef _WIN32
		return gethostbyaddr((const char*)addr,sizeof(sockaddr_in),AF_INET);
		#else
		return gethostbyaddr(addr,sizeof(sockaddr_in),AF_INET);
		#endif
	};
	//IPV6版本
	inline hostent *os_gethostbyaddr(const AddrV6 *addr){
		#ifdef _WIN32
		return gethostbyaddr((const char*)addr,sizeof(sockaddr_in6),AF_INET6);
		#else
		return gethostbyaddr(addr,sizeof(sockaddr_in6),AF_INET6);
		#endif
	};
	//得到主机信息通过host
	inline hostent *os_gethostbyname(const char *name){
		return gethostbyname(name);
	};
	#ifdef _WIN32
	//Window inet_ptoa的实现 ntop
	//IP转地址
	int inet_pton(int type,const char *str,void *addr){
		int len;//地址长度
		if(type == AF_INET){
			len = sizeof(sockaddr_in);
		}
		else{
			len = sizeof(sockaddr_in6);
		}
		//我不知道这个函数会不会改变原本字符串 那个值没有被const
		return WSAStringToAddress((char*)str,type,nullptr,(sockaddr*)addr,&len);
	}
	//地址转IP
	int inet_ntop(int type,const void *addr,char *buf,unsigned long buflen){
		int len;//地址长度
		if(type == AF_INET){
			len = sizeof(sockaddr_in);
		}
		else{
			len = sizeof(sockaddr_in6);
		}
		//这里也是 不知道addr会不会被改变 还有那个最后一个参数 LPDWORD
		return WSAAddressToString((sockaddr*)addr,len,nullptr,buf,&buflen);
	}
	#endif
};
//内连函数
void BaseSocket::set_fd(NativeSocket fd){
	//从fd中生成文件流
	this->fd = fd;
	fin = fdopen(fd,"rb");
	fout = fdopen(dup(fd),"wb");
	if(fin == nullptr or fout == nullptr){
		//失败了
		::throw_os_error();
	}
}
BaseSocket::BaseSocket(){
	//设置所有的变量
	fin = nullptr;
	fout = nullptr;
	fd = -1;
}
BaseSocket::BaseSocket(NativeSocket fd){
	set_fd(fd);
}
BaseSocket::~BaseSocket(){
	//关闭流
	this->close();
}
void BaseSocket::close(){
	//关掉
	if(fin != nullptr){
		fclose(fin);
		fin = nullptr;
	}
	if(fout != nullptr){
		fclose(fout);
		fout = nullptr;
	}
}
void BaseSocket::flush(){
	if(fflush(fout) != 0){
		//失败
		::throw_os_error();
	}
}
//读写
size_t BaseSocket::read(void *buf,size_t size){
	auto ret = fread(buf,1,size,fin);
	if(ferror(fin) != 0){
		//读入失败
		ReadError err;
		err.read_size = ret;
		err.code = (ErrorCode)errno;
		err.msg = strerror(errno);
		throw err;
	}
	return ret;
}
//写
size_t BaseSocket::write(const void *buf,size_t size){
	auto ret = fwrite(buf,1,size,fout);
	if(ferror(fout) != 0){
		WriteError err;
		err.write_size = ret;
		err.code = (ErrorCode)errno;
		err.msg = strerror(errno);
		throw err;
	}
	return ret;
}
//打印输出
int BaseSocket::printf(const char *fmt,...) noexcept{
	va_list varg;
	va_start(varg,fmt);
	int ret = vfprintf(fout,fmt,varg);
	va_end(varg);
	return ret;
}
//输入
int BaseSocket::scanf(const char *fmt,...) noexcept{
	va_list varg;
	va_start(varg,fmt);
	int ret = vfscanf(fin,fmt,varg);
	va_end(varg);
	return ret;
}
//一行
char* BaseSocket::gets(char *buf,int bufsize) noexcept{
	return fgets(buf,bufsize,fin);
}
//打印一行
int BaseSocket::puts(const char *text) noexcept{
	return fputs(text,fout);
}
int BaseSocket::getc() noexcept{
	return fgetc(fin);
}
int BaseSocket::putc(int ch) noexcept{
	return fputc(ch,fout);
}
//放回去
int BaseSocket::ungetc(int ch) noexcept{
	return ::f_ungetc(ch,fin);
}
//设置不堵塞
void BaseSocket::set_nonblock(bool val){
	#ifdef _WIN32
	u_long arg;
	if(val == true){
		//不堵塞
		arg = 0;
	}
	else{
		arg = 1;
	}
	if(ioctlsocket(fd,FIONBIO,&arg) !=0){
		//失败
		throw_os_error();
	}
	#else
	//得到flags
	int flags = fcntl(fd, F_GETFL, 0);
	if(flags < 0){
		throw_os_error();
	}
	if(val == true){
		//不堵塞
		flags = flags | O_NONBLOCK;
	}
	else{
		flags = flags ^ O_NONBLOCK;
	}
	if(fcntl(fd,F_SETFL,flags) < 0){
		throw_os_error();//失败
	}
	#endif
}
//绑定地址
void BaseSocket::bind(const AddrV4 &addr){
	//跳到SOCK BIND
	::sock_bind(fd,addr);
}
//IPV6
void BaseSocket::bind(const AddrV6 &addr){
	::sock_bind(fd,addr);
}
//听
void BaseSocket::listen(int backlog){
	::sock_listen(fd,backlog);
}
//连接
void BaseSocket::connect(const AddrV4 &addr){
	::sock_connect(fd,addr);
}
//IPV6的连接
void BaseSocket::connect(const AddrV6 &addr){
	::sock_connect(fd,addr);
}
//得到Socketd的地址
AddrV4 BaseSocket::get_addrv4_name()const{
	AddrV4 addr;
	get_name(addr);
	return addr;
}
AddrV6 BaseSocket::get_addrv6_name()const{
	AddrV6 addr;
	get_name(addr);
	return addr;
}
//IPV4版本
void BaseSocket::get_name(AddrV4 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		::throw_os_error();
	}
}
//IPV6版本
void BaseSocket::get_name(AddrV6 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in6);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in6);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		::throw_os_error();
	}
}
//得到与他相连的Socket名字
void BaseSocket::get_peer_name(AddrV4 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		::throw_os_error();
	}
}
//IPV6
void BaseSocket::get_peer_name(AddrV6 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in6);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in6);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		::throw_os_error();
	}
}
//OS API 接受数据和发送数据

ssize_t BaseSocket::sendto(const void *buf,size_t buflen,int flags,const AddrV4 *addr) noexcept{
	return ::sock_sendto(fd,buf,buflen,flags,addr);
}
ssize_t BaseSocket::recvfrom(void *buf,size_t buflen,int flags,AddrV4 *addr) noexcept{
	return ::sock_recvfrom(fd,buf,buflen,flags,addr);
}

//得到文件描述符号
NativeSocket BaseSocket::get_fd() const{
	return fd;
}
//输入输出流
FILE *BaseSocket::in() const{
	return fin;
}
FILE *BaseSocket::out() const{
	return fout;
}
size_t BaseSocket::operator <<(const std::string & str){
	return this->write(str.c_str(),str.length() * sizeof(char));
}
size_t BaseSocket::operator >>(std::string &s){
	char buf[64];//缓冲区
	char *endptr;//结束字符指针
	size_t byte = 0;//读入字节
	size_t retbyte;
	while(true){
		endptr = this->gets(buf,sizeof(buf));
		if(endptr == nullptr){
			//失败
			ReadError err;
			err.read_size = byte;
			err.code = (ErrorCode)errno;
			err.msg = strerror(errno);
			throw err;
		}
		else{
			//读入大小
			retbyte = strlen(buf);
			s.append(buf,retbyte);
			byte += retbyte;
			if(strchr(buf,'\n') != nullptr){
				//读到了换行
				break;
			}
		}
	}
	return byte;
}



//地址
AddrV4::AddrV4(){
	//清空自己
	this->clear();
}
AddrV4::AddrV4(const std::string &ip,uint16_t port){
	//通过ip和port得到地址
	this->clear();
	sin_port = htons(port);
	sin_addr.s_addr = inet_addr(ip.c_str());
	//sin_addr = (ip.c_str());;
}
//从IP构建
AddrV4 AddrV4::From(const std::string &ip,uint16_t port){
	return AddrV4(ip,port);
}
//从主机名字构建
AddrV4 AddrV4::FromHost(const std::string &host,uint16_t port){
	AddrV4 addr;
	hostent *ent = ::os_gethostbyname(host.c_str());
	if(ent == nullptr){
		//失败
		throw_os_netdb_error();
	}
	//复制地址
	memcpy(&(addr.sin_addr),(struct in_addr*)ent->h_addr_list[0],sizeof(in_addr));
	//设置IP
	//设置家族
	//addr.sin_family = ent->h_addrtype;
	addr.sin_port = htons(port);
	return addr;
}
void AddrV4::clear() noexcept{
	memset(this,0,sizeof(AddrV4));
	sin_family = AF_INET;
}
//设置IP
void AddrV4::set_ip(const std::string &ip) noexcept{
	sin_addr.s_addr = inet_addr(ip.c_str());
}
//设置Port
void AddrV4::set_port(uint16_t port) noexcept{
	sin_port = htons(port);
}
//得到地址和其他东西
std::string AddrV4::get_host() const{
	hostent *ent = ::os_gethostbyaddr(this);
	//得到主机名字
	if(ent == nullptr){
		//错误
		throw_os_netdb_error();
	}
	return std::string(ent->h_name);
}
std::string AddrV4::get_ip() const{
	return inet_ntoa(sin_addr);
}
uint16_t AddrV4::get_port() const{
	return ntohs(sin_port);
}

//IPV6地址
AddrV6::AddrV6(){
	this->clear();
}
AddrV6::AddrV6(const std::string &ip,uint16_t port){
	inet_pton(AF_INET6,ip.c_str(),&sin6_addr);
	//转换IP到里面去
	//转换Port
	sin6_port = htons(port);
}
AddrV6 AddrV6::From(const std::string &ip,uint16_t port){
	return AddrV6(ip,port);
}
//设置IP
void AddrV6::set_ip(const std::string &ip) noexcept{
	inet_pton(AF_INET6,ip.c_str(),&sin6_addr);
}
//设置端口
void AddrV6::set_port(uint16_t port) noexcept{
	sin6_port = htons(port);
}
void AddrV6::clear() noexcept{
	//清空 并设置协议家族
	memset(this,0,sizeof(sockaddr_in6));
	sin6_family = AF_INET6;
}
//得到一些参数
std::string AddrV6::get_host() const{
	hostent *ent = ::os_gethostbyaddr(this);
	//得到主机名字
	if(ent == nullptr){
		//错误
		throw_os_netdb_error();
	}
	return std::string(ent->h_name);
}
//得到IP地址
std::string AddrV6::get_ip() const{
	char buf[INET6_ADDRSTRLEN];//缓冲区
	inet_ntop(AF_INET6,&sin6_addr,buf,sizeof(buf));
	return std::string(buf);
}
uint16_t AddrV6::get_port() const{
	return ntohs(sin6_port);
}

//TCP
TCP::TCP(Type type){
	NativeSocket fd;
	if(type == Type::IPV4){
		//创建
		fd = socket(AF_INET,SOCK_STREAM,0);
	}
	else{
		fd = socket(AF_INET6,SOCK_STREAM,0);
	}
	if(fd < 0){
		//失败
		::throw_os_error();
	}
	//设置一下
	set_fd(fd);
}
//接受连接
TCP *TCP::accept(AddrV4 *addr){
	auto ret = ::sock_accept(fd,addr);
	if(ret < 0){
		return nullptr;
	}
	return (TCP*)new BaseSocket(ret);
}
//流式Socket的接受函数
ssize_t TCP::recv(void *buf,size_t buflen,int flags) noexcept{
	return ::sock_recv(fd,buf,buflen,flags);
}
ssize_t TCP::send(const void *buf,size_t buflen,int flags) noexcept{
	return ::sock_send(fd,buf,buflen,flags);
}
//UDP SOCKET
UDP::UDP(Type type){
	NativeSocket fd;
	if(type == Type::IPV4){
		fd = socket(AF_INET,SOCK_DGRAM,0);
	}
	else{
		fd = socket(AF_INET6,SOCK_DGRAM,0);
	}
	if(fd < 0){
		//失败
		::throw_os_error();
	}
	//设置fd
	set_fd(fd);
}
//Set
Set::Set(){
	this->clear();
}
void Set::clear(){
	FD_ZERO(this);
	#ifndef _WIN32
	max_fd = -1;
	#endif
}
void Set::add(BaseSocket &sock){
	FD_SET(sock.fd,this);
	//设置fd
	#ifndef _WIN32
	if(sock.fd > max_fd){
		max_fd = sock.fd;
	}
	#endif
}
bool Set::is_set(BaseSocket &sock){
	return FD_ISSET(sock.fd,this);
}
//Select
int Socket::Select(Set *r_set,Set *w_set,Set *e_set,const timeval *t){
	#ifdef _WIN32
	if(t != nullptr){
		struct timeval teval = *t;
		return select(0,r_set,w_set,e_set,&teval);
	}
	else{
		return select(0,r_set,w_set,e_set,nullptr);
	}
	#else
	NativeSocket max_fd = -1;
	//查找最大的fd
	if(r_set != nullptr){
		if(r_set->max_fd > max_fd){
			max_fd = r_set->max_fd;
		}
	}
	if(w_set != nullptr){
		if(w_set->max_fd > max_fd){
			max_fd = w_set->max_fd;
		}
	}
	if(e_set != nullptr){
		if(e_set->max_fd > max_fd){
			max_fd = e_set->max_fd;
		}
	}
	if(t != nullptr){
		//不堵塞
		struct timeval teval = *t;
		return select(max_fd + 1,r_set,w_set,e_set,&teval);
	}
	else{
		return select(max_fd + 1,r_set,w_set,e_set,nullptr);
	}
	#endif
}
int Socket::GetErrorCode(){
	//得到错误代码
	#ifdef _WIN32
		return WSAGetLastError();
	#else
		return errno;
	#endif
}
const char *Socket::GetError(){
	//得到错误
	#ifdef _WIN32
		return strerror(WSAGetLastError());
	#else
		return strerror(errno);
	#endif
}

//一对TCPSocket
void Socket::CreatePair(BaseSocket **t1,BaseSocket **t2){
	#ifdef _WIN32
	//Window是自己的实现
	try{
		TCP tcp(Type::IPV4);
		*t2 = new TCP(Type::IPV4);//初始化第二个Socket
		tcp.listen(1);
		AddrV4 addr;
		tcp.get_name(addr);
		//得到主机地址
		(*t2)->connect(addr);//连接
		*t1 = tcp.accept();//结束
		return;
	}
	catch(OSError err){
		//回收资源
		if(*t1 != nullptr){
			delete *t1;
		}
		if(*t2 != nullptr){
			delete *t2;
		}
		throw;//再抛出
	}
	#else
	NativeSocket fds[2];
	if(socketpair(AF_UNIX,SOCK_STREAM,0,fds) != 0){
		//失败
		::throw_os_error();
	}
	*t1 = new BaseSocket();
	*t2 = new BaseSocket();
	//创建Socket
	(*t1)->set_fd(fds[0]);
	(*t2)->set_fd(fds[1]);
	#endif
}
