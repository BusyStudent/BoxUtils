#include "socket.hpp"
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
using namespace BoxUtils;
//内连函数
static inline int sock_close(int fd){
	//关闭
	return close(fd);
}
static inline int sock_open(int domain,int protocol){
	//申请一个Socket
	return socket(domain,protocol,0);
}
static inline int sock_bind(int fd,void *addr,socklen_t len){
	//绑定
	return bind(fd,(struct sockaddr*)addr,len);
}
static inline int sock_listen(int fd,int backlog){
	//监听
	return listen(fd,backlog);
}
static inline ssize_t sock_write(int fd,const void *buf,size_t len){
	//写
	return write(fd,buf,len);
}
static inline ssize_t sock_read(int fd,void *buf,size_t len){
	//读入
	return read(fd,buf,len);
}
static inline int sock_accept(int fd,void *addr,socklen_t *len){
	return accept(fd,(struct sockaddr*)addr,len);
}
//内置函数
Socket::~Socket(){
	sock_close(fd);
}
int Socket::get_fd(){
	//得到fd
	return fd;
}
const char *Socket::GetError(){
	//得到错误
	return strerror(errno);
}
//绑定
bool Socket::bind(SockAddress &addr){
	if(sock_bind(fd,&(addr.addr),addr.len()) < 0){
		//绑定错误
		throw_for_errno();
		return false;
	}
	return true;
}
bool Socket::bind(const char *ip,uint16_t port){
	SockAddress addr(ip,port);
	return bind(addr);
}
bool Socket::listen(int backlog){
	if(sock_listen(fd,backlog) < 0){
		throw_for_errno();
		return false;
	}
	return true;
}
bool Socket::close(){
	if(sock_close(fd) < 0){
		//失败
		throw_for_errno();
		return false;
	}
	else{
		return true;
	}
}
ssize_t Socket::write(const void *buf,size_t len){
	//写
	auto ret = sock_write(fd,buf,len);
	if(ret < 0){
		//失败
		throw_for_errno();
	}
	return ret;
}
ssize_t Socket::read(void *buf,size_t len){
	auto ret = sock_read(fd,buf,len);
	if(ret < 0){
		throw_for_errno();
	}
	return ret;
}
//通过errno抛出异常
void Socket::throw_for_errno(){
	if(noexcept_){
		//不抛出异常
		return;
	}
	switch(errno){
		case EINVAL:
			//非法参数
			throw std::invalid_argument(GetError());
		default:
			//未知类型一致抛出Runtime
			throw std::runtime_error(GetError());
	}
}
void Socket::set_noblock(){
	int flags = fcntl(fd,F_GETFL,0);
	//获取flags
	fcntl(fd,F_SETFL,flags | O_NONBLOCK);
	//不堵塞
}
void Socket::set_noexcept(){
	noexcept_ = true;
}
//地址
SockAddress::SockAddress(){
	//初始化清空
	memset(this,0,sizeof(SockAddress));
}
SockAddress::SockAddress(const char *ip,uint16_t port){
	//通过地址来构建
	auto slen = strlen(ip);
	//字符串长度
	if(slen > 46){
		//IPV6
		type = V6;
	}
	else{
		//IPV4
		type = V4;
		addr.v4.sin_family = AF_INET;//设置协议家族
		addr.v4.sin_port = htons(port);
		addr.v4.sin_addr.s_addr = inet_addr(ip);
	}
}
SockAddress::SockAddress(const struct sockaddr_in &v4_addr){
	//通过一个V4结构体来构建Address
	LoadStruct(*this,v4_addr);
}
SockAddress::SockAddress(const struct sockaddr_in6 &v6_addr){
	//IPV6
	memcpy(&(addr.v6),&v6_addr,sizeof(sockaddr_in6));
	type = V6;
}
void SockAddress::LoadStruct(SockAddress& addr,const struct sockaddr_in &v4_addr){
	memcpy(&(addr.addr.v4),&v4_addr,sizeof(struct sockaddr_in));
	addr.type = V4;
}
std::string SockAddress::get_ip(){
	switch(type){
		case V6:
			//待完成
			//return inet6_ntop();
			abort();
		case V4:
			return inet_ntoa(addr.v4.sin_addr);
		default:
			throw std::invalid_argument("Unkown Address Type");
	}
}
uint16_t SockAddress::get_port(){
		switch(type){
		case V6:
			return htons(addr.v6.sin6_port);
		case V4:
			return htons(addr.v4.sin_port);
		default:
			throw std::invalid_argument("Unkown Address Type");
	}
}
socklen_t SockAddress::len(){
	switch(type){
		//判断类型
		case V4:
			//IPV4地址长度
			return sizeof(struct sockaddr_in);
		case V6:
			return sizeof(struct sockaddr_in6);
		default:
			throw std::invalid_argument("Unkown Address Type");
	}
}
//TCP
TCPSocket::TCPSocket(bool v4){
	if(v4){
		//IPV4
		fd = sock_open(AF_INET,SOCK_STREAM);
	}
	else{
		//IPV6
		fd = sock_open(AF_INET6,SOCK_STREAM);
	}
	if(fd < 0){
		//失败
		throw std::bad_alloc();
	}
}
TCPSocket *TCPSocket::accept(SockAddress *addr){
	//接入链接
	struct sockaddr_in v4_addr;
	//地址
	socklen_t addrlen = sizeof(struct sockaddr_in);
	auto new_fd = sock_accept(fd,&v4_addr,&addrlen);
	//接入链接
	if(new_fd < 0){
		//失败
		throw_for_errno();
		return nullptr;
	}
	//
	if(addr != nullptr){
		//复制地址
		SockAddress::LoadStruct(*addr,v4_addr);
	}
	TCPSocket *sock = (TCPSocket*)new Socket();
	sock->fd = new_fd;
	return sock;
}
