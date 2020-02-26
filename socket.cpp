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
	return write(fd,buf,len);
}
static inline ssize_t sock_read(int fd,void *buf,size_t len){
	return read(fd,buf,len);
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
		default:
			//未知类型一致抛出Runtime
			throw std::runtime_error(GetError());
	}
}
void Socket::set_noexcept(){
	noexcept_ = true;
}
//地址
SockAddress::SockAddress(){
	//初始化清空
	memset(this,0,sizeof(SockAddress));
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
