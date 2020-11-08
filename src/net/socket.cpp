//#define _BOX_SOURCE
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include "common/def.hpp"
#include "raii/scope.hpp"
#include "net/socket.hpp"
#include "backtrace.hpp"
#include "exception.hpp"
#include "fmt.hpp"
#include "libc/mem.h"
#include "libc/inet.h"
#include "libc/atexit.h"
using namespace Box::Net;
using namespace Box;
namespace{
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
};
//内连函数
Socket::Socket(NativeSocket fd):SocketRef(fd){

}
Socket::Socket(Socket &&sock):
	SocketRef(sock.detach_fd()){
	//移动函数
}
Socket::~Socket(){
	//关闭流
	this->close();
}
void SocketRef::close(){
	//关掉
	if(not BOX_SOCKET_INVAID(fd)){
		libc::closesocket(fd);
		//设置会非法套接子
		#ifdef _WIN32
		fd = INVALID_SOCKET;
		#else
		fd = -1;
		#endif
	}
}
void SocketRef::shutdown(Shut how){
	if(::shutdown(fd,static_cast<int>(how)) == -1){
		throwSocketError();
	}
}
//读写
ssize_t SocketRef::recv(void *buf,size_t buflen,int flags) noexcept{
	return libc::recv(fd,buf,buflen,flags);
}
ssize_t SocketRef::send(const void *buf,size_t buflen,int flags) noexcept{
	return libc::send(fd,buf,buflen,flags);
}
//设置不堵塞
void SocketRef::set_nonblock(bool val){
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
		throwSocketError();
	}
	#else
	//得到flags
	int flags = fcntl(fd, F_GETFL, 0);
	if(flags < 0){
		throwSocketError();
	}
	if(val == true){
		//不堵塞
		flags = flags | O_NONBLOCK;
	}
	else{
		flags = flags ^ O_NONBLOCK;
	}
	if(fcntl(fd,F_SETFL,flags) < 0){
		throwSocketError();//失败
	}
	#endif
}
//设置可以继承
void SocketRef::set_inherit(bool val){
	#ifdef _WIN32

	#else
	int flags = fcntl(fd,F_GETFL,0);
	if(flags < 0){
		throwSocketError();
	}
	if(val){
		if(not BOX_HASBIT(flags,O_CLOEXEC)){
			//已经设置过了
			return;
		}
		flags ^= O_CLOEXEC;
	}
	else{
		if(BOX_HASBIT(flags,O_CLOEXEC)){
			return;
		}
		flags |= O_CLOEXEC;
	}
	//设置flags
	if(fcntl(F_SETFL,flags) < 0){
		throwSocketError();
	}
	#endif
}
//是否可继承
bool SocketRef::is_inherit() const{
	#ifdef _WIN32

	#else
	int flags = fcntl(fd,F_GETFL,0);
	if(flags < 0){
		throwSocketError();
	}
	return not BOX_HASBIT(flags,O_CLOEXEC);
	#endif
}
//绑定地址
void SocketRef::bind(const AddrV4 &addr){
	if(libc::bind(fd,&addr,sizeof(sockaddr_in)) != 0){
		throwSocketError();
	}
}
//IPV6
void SocketRef::bind(const AddrV6 &addr){
	if(libc::bind(fd,&addr,sizeof(sockaddr_in6)) != 0){
		throwSocketError();
	}
}
//OS
void SocketRef::bind(const void *addr,size_t addrsize){
	if(libc::bind(fd,addr,addrsize) != 0){
		throwSocketError();
	}
}
//听
void SocketRef::listen(int backlog){
	int ret = ::listen(fd,backlog);
	if(ret != 0){
		//失败
		throwSocketError();
	}
}
//连接
void SocketRef::connect(const AddrV4 &addr){
	if(libc::connect(fd,&addr,sizeof(sockaddr_in)) != 0){
		throwSocketError();
	}
}
//IPV6的连接
void SocketRef::connect(const AddrV6 &addr){
	if(libc::connect(fd,&addr,sizeof(sockaddr_in6)) != 0){
		throwSocketError();
	}
}
//原始连接
void SocketRef::connect(const void *addr,size_t addrsize){
	if(libc::connect(fd,addr,addrsize) != 0){
		throwSocketError();
	}
}
//得到Socketd的地址
AddrV4 SocketRef::get_addrv4_name()const{
	AddrV4 addr;
	get_name(addr);
	return addr;
}
AddrV6 SocketRef::get_addrv6_name()const{
	AddrV6 addr;
	get_name(addr);
	return addr;
}
//IPV4版本
void SocketRef::get_name(AddrV4 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//IPV6版本
void SocketRef::get_name(AddrV6 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in6);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in6);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//通过地址大小的到
void SocketRef::get_name(void *addr,size_t addrsize) const{
	#ifdef _WIN32
	int len = static_cast<int>(addrsize);
	auto code = getsockname(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = static_cast<socklen_t>(addrsize);
	auto code = getsockname(fd,static_cast<sockaddr*>(addr),&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//得到与他相连的Socket名字
void SocketRef::get_peer_name(AddrV4 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//IPV6
void SocketRef::get_peer_name(AddrV6 &addr)const{
	#ifdef _WIN32
	int len = sizeof(sockaddr_in6);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#else
	socklen_t len = sizeof(sockaddr_in6);
	auto code = getpeername(fd,(sockaddr*)&addr,&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//原始的接口
void SocketRef::get_peer_name(void *addr,size_t addrsize) const{
	#ifdef _WIN32
	int len = static_cast<int>(addrsize);
	auto code = getpeername(fd,static_cast<sockaddr*>(addr),&len);
	#else
	socklen_t len = static_cast<socklen_t>(addrsize);
	auto code = getpeername(fd,static_cast<sockaddr*>(addr),&len);
	#endif
	if(code != 0){
		//失败
		throwSocketError(BOX_H_ERRNO);
	}
}
//OS API 接受数据和发送数据

ssize_t SocketRef::sendto(const void *buf,size_t buflen,int flags,const AddrV4 *addr) noexcept{
	return libc::sendto(fd,buf,buflen,flags,(const sockaddr*)(addr),sizeof(sockaddr_in));
}
ssize_t SocketRef::recvfrom(void *buf,size_t buflen,int flags,AddrV4 *addr) noexcept{
	libc::socklen_t len = sizeof(sockaddr_in);
	return libc::recvfrom(fd,buf,buflen,flags,(sockaddr*)addr,&len);
}

//得到文件描述符号
NativeSocket SocketRef::get_fd() const{
	return fd;
}
//分离描述符号
NativeSocket Socket::detach_fd(){
	NativeSocket sock = fd;
	#ifdef _WIN32
	fd = INVALID_SOCKET;
	#else
	fd = -1;
	#endif
	return sock;
}
ssize_t SocketRef::operator <<(std::string_view str){
	return this->send(str.data(),str.length() * sizeof(char));
}
//复制Socket
Socket Socket::dup(){
	NativeSocket s;
	#ifdef _WIN32
	WSAPROTOCOL_INFO info;
	if(WSADuplicateSocket(
		fd,
		GetCurrentProcessId(),
		&info
	) != 0){
		throwSocketError();
	}
	//创建Socket
	s = WSASocket(0,0,0,&info,0,0);
	#else
	s = ::dup(fd);
	#endif
	//出错
	if(BOX_SOCKET_INVAID(s)){
		throwSocketError();
	}
	return Socket(s);
}
//创建Socket
NativeSocket Socket::Create(int domain,int type,int prot){
	#ifndef _WIN32
	//不给继承
	type |= SOCK_CLOEXEC;
	#endif
	NativeSocket sock = socket(domain,type,prot);
	//创建一下
	if(BOX_SOCKET_INVAID(sock)){
		//失败
		#ifdef _WIN32
		int code = WSAGetLastError();
		if(code == WSANOTINITIALISED){
			//如果没初始化
			libc::socket_init();
			libc::atexit_once(libc::socket_quit);
			sock = socket(domain,type,prot);
			//再申请一次
			if(BOX_SOCKET_INVAID(sock)){
				throwSocketError();
			}
		}
		else{
			throwSocketError();
		}
		#else
		throwSocketError();
		#endif
	}
	return sock;
}
//地址
AddrV4::AddrV4(){
	//清空自己
	this->clear();
}
AddrV4::AddrV4(std::string_view ip,uint16_t port){
	//通过ip和port得到地址
	this->clear();
	sin_port = htons(port);
	sin_addr.s_addr = inet_addr(ip.data());
	//sin_addr = (ip.c_str());;
}
//从IP构建
AddrV4 AddrV4::From(std::string_view ip,uint16_t port){
	return AddrV4(ip,port);
}
//从主机名字构建
AddrV4 AddrV4::FromHost(std::string_view host,uint16_t port){
	AddrV4 addr;
	hostent *ent = ::gethostbyname(host.data());
	if(ent == nullptr){
		//失败
		throwSocketError(BOX_H_ERRNO);
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
void AddrV4::set_ip(std::string_view ip) noexcept{
	sin_addr.s_addr = inet_addr(ip.data());
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
		throwSocketError(BOX_H_ERRNO);
	}
	return std::string(ent->h_name);
}
std::string AddrV4::get_ip() const{
	return inet_ntoa(sin_addr);
}
uint16_t AddrV4::get_port() const{
	return ntohs(sin_port);
}
std::string AddrV4::to_string() const{
	return Format("{}:{}",get_ip(),get_port());
}
//比较地址
bool AddrV4::operator ==(const AddrV4 &addr) const{
	return memcmp(this,&addr,sizeof(AddrV4)) == 0;
}
bool AddrV4::operator !=(const AddrV4 &addr) const{
	return memcmp(this,&addr,sizeof(AddrV4)) != 0;
}

//IPV6地址
AddrV6::AddrV6(){
	this->clear();
}
AddrV6::AddrV6(std::string_view ip,uint16_t port){
	inet_pton(AF_INET6,ip.data(),&sin6_addr);
	//转换IP到里面去
	//转换Port
	sin6_port = htons(port);
}
AddrV6 AddrV6::From(std::string_view ip,uint16_t port){
	return AddrV6(ip,port);
}
//设置IP
void AddrV6::set_ip(std::string_view ip) noexcept{
	inet_pton(AF_INET6,ip.data(),&sin6_addr);
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
		throwSocketError(BOX_H_ERRNO);
	}
	return std::string(ent->h_name);
}
//得到IP地址
std::string AddrV6::get_ip() const{
	char buf[INET6_ADDRSTRLEN];//缓冲区
	libc::inet_ntop(AF_INET6,&sin6_addr,buf,sizeof(buf));
	return std::string(buf);
}
uint16_t AddrV6::get_port() const{
	return ntohs(sin6_port);
}
std::string AddrV6::to_string() const{
	return Format("{}:{}",get_ip(),get_port());
}
//比较地址
bool AddrV6::operator ==(const AddrV6 &addr) const{
	return memcmp(this,&addr,sizeof(AddrV6)) == 0;
}
bool AddrV6::operator !=(const AddrV6 &addr) const{
	return memcmp(this,&addr,sizeof(AddrV6)) != 0;
}

//TCP
Tcp::Tcp(SockFamily family)
	:Socket(Create((int)family,SOCK_STREAM,0)){
		//创建一个TCPSocket
}
//接受连接
Socket *SocketRef::accept(AddrV4 *addr){
	libc::socklen_t len = sizeof(sockaddr_in);
	libc::socket_t ret = libc::accept(fd,addr,&len);
	if(BOX_SOCKET_INVAID(ret)){
		//不是有效的
		return nullptr;
	}
	return new Socket(ret);
}
//UDP SOCKET
Udp::Udp(SockFamily family):
	Socket(Create((int)family,SOCK_DGRAM,0)){
		//创建一个UDP Socket
}
//Set
SockSet::SockSet(){
	this->clear();
}
void SockSet::clear(){
	FD_ZERO(this);
	#ifndef _WIN32
	max_fd = -1;
	#endif
}
void SockSet::add(SocketRef sock){
	FD_SET(sock.fd,this);
	//设置fd
	#ifndef _WIN32
	if(sock.fd > max_fd){
		max_fd = sock.fd;
	}
	#endif
}
bool SockSet::is_set(SocketRef sock){
	return FD_ISSET(sock.fd,this);
}
int Socket::Poll(NativePollfd fds[],size_t n,int timeout) noexcept{
	return BOX_POLL(fds,n,timeout);
}
//Select
int Socket::Select(SockSet *r_set,SockSet *w_set,SockSet *e_set,const timeval *t) noexcept{
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
std::string Socket::GetError(){
	//得到错误
	#ifdef _WIN32
		char *buf = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buf,
			0,
			nullptr);
		//格式化错误信息
		std::string msg(buf);
		LocalFree(buf);
		return msg;
	#else
		return strerror(errno);
	#endif
}

//一对TCPSocket
void Socket::Pair(Socket *socks[2] ){
	#ifdef _WIN32
	//Window是自己的实现
	socks[0] = nullptr;
	socks[1] = nullptr;
	ScopeGuard guard([&](){
		//抛出异常时候删除这几个
		delete socks[0];
		delete socks[1];
	});
	Tcp tcp(SockFamily::IPV4);
	socks[1] = new Tcp(SockFamily::IPV4);//初始化第二个Socket
	tcp.listen(1);
	AddrV4 addr;
	tcp.get_name(addr);
	//得到主机地址
	socks[1]->connect(addr);//连接
	socks[0] = tcp.accept();//结束
	//释放guard
	guard.release();
	return;
	#else
	NativeSocket fds[2];
	if(socketpair(AF_UNIX,SOCK_STREAM,0,fds) != 0){
		//失败
		throwSocketError();
	}
	socks[0] = new Socket(fds[0]);
	socks[1] = new Socket(fds[1]);
	//创建Socket
	#endif
}
namespace Box{
namespace Net{
	//SocketError
	//格式化错误
	std::string FormatError(int code){
		using Box::Format;
		char *os_msg;//系统错误
		#ifdef _WIN32
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&os_msg,
			0,
			nullptr);
		//转换SocketError
		#else
		os_msg = strerror(code);
		#endif
		std::string ret = Format("[SockError {}] {}",code,os_msg);
		#ifdef _WIN32
		LocalFree(os_msg);
		#endif
		return ret;
	}
	SocketError::SocketError(int code):
		errcode(code),msg(FormatError(code)){
		//初始化错误
	}
	SocketError::SocketError(const SocketError &err):
		errcode(err.errcode),msg(err.msg){

	}
	SocketError::~SocketError(){}
	//输出
	const char *SocketError::what() const noexcept{
		return msg.c_str();
	}
	BOXAPI [[noreturn]] void throwSocketError(int code){
		throw SocketError(code);
	}
};
};