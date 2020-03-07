#include "socket.hpp"
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
using namespace Box;
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
static inline ssize_t sock_recvfrom(int fd,void *buf,
									size_t len,int flag,
									void *addr,socklen_t *slen){
	return recvfrom(fd,buf,len,flag,(struct sockaddr*)addr,slen);
}
static inline ssize_t sock_sendto(int fd,const void *buf,
								size_t len,int flag,
								void *addr,socklen_t slen){
	return sendto(fd,buf,len,flag,(struct sockaddr*)addr,slen);
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
void SockAddress::LoadStruct(SockAddress& addr,const struct sockaddr_in6 &v6_addr){
	memcpy(&(addr.addr.v6),&v6_addr,sizeof(struct sockaddr_in6));
	addr.type = V6;
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
		throw_for_errno();
	}
	this->v4 = v4;//设置是否是IPV4
}
TCPSocket *TCPSocket::accept(SockAddress *addr){
	//接入链接
	int new_fd;
	if(v4){
		//IPV4
		struct sockaddr_in v4_addr;
		//地址
		socklen_t addrlen = sizeof(struct sockaddr_in);
		new_fd = sock_accept(fd,&v4_addr,&addrlen);
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
	}
	else{
		//IPV6
		struct sockaddr_in6 v6_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in6);
		new_fd = sock_accept(fd,&v6_addr,&addrlen);
		if(new_fd < 0){
			//是白
			throw_for_errno();
			return nullptr;
		}
		if(addr != nullptr){
			SockAddress::LoadStruct(*addr,v6_addr);
		}
	}
	TCPSocket *sock = (TCPSocket*)new Socket();
	sock->fd = new_fd;
	return sock;
}
//UDPSocket
UDPSocket::UDPSocket(bool v4){
	if(v4){
		//IPV4 UDP
		fd = sock_open(AF_INET,SOCK_DGRAM);
	}
	else{
		//IPV6 UDP
		fd = sock_open(AF_INET6,SOCK_DGRAM);
	}
	if(fd < 0){
		//失败 抛出异常
		throw_for_errno();
	}
	this->v4 = v4;
}
ssize_t UDPSocket::recvfrom(void *buf,size_t len,SockAddress *addr){
	//UDP接收数据
	ssize_t ret;//接受数据大小
	if(v4){
		//IPV4
		struct sockaddr_in v4_addr;
		socklen_t addrlen = sizeof(sockaddr_in);
		//地址和大小
		ret = sock_recvfrom(fd,buf,len,0,&v4_addr,&addrlen);
		if(ret < 0){
			//失败
			throw_for_errno();
			return ret;
		}
		if(addr != nullptr){
			SockAddress::LoadStruct(*addr,v4_addr);
		}
	}
	else{
		//IPV6
		struct sockaddr_in6 v6_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in6);
		ret = sock_recvfrom(fd,buf,len,0,&v6_addr,&addrlen);
		if(ret < 0){
			throw_for_errno();
			return ret;
		}
		if(addr != nullptr){
			//赋值地址
			SockAddress::LoadStruct(*addr,v6_addr);
		}
	}
	return ret;
}
ssize_t UDPSocket::recvfrom(void *buf,size_t len,const char **ip,uint16_t *port){
	SockAddress addr;
	auto ret = this->recvfrom(buf,len,&addr);
	//调用底层
	if(ret < 0){
		//出错而且没有抛出异常 直接返回
		return ret;
	}
	if(ip != nullptr){
		*ip = strdup(addr.get_ip().c_str());
	}
	if(port != nullptr){
		*port = addr.get_port();
	}
	return ret;
}
ssize_t UDPSocket::sendto(const void *buf,size_t len,SockAddress &addr){
	//发送数据
	auto ret = sock_sendto(fd,buf,len,0,&(addr.addr),addr.len());
	if(ret < 0){
		//出错
		throw_for_errno();
	}
	return ret;
}
ssize_t UDPSocket::sendto(const void *buf,size_t len,const char *ip,uint16_t port){
	SockAddress addr(ip,port);
	return this->sendto(buf,len,addr);
}
//SocketSet
SocketSet::SocketSet(){
	this->clear();//清空自己
}
void SocketSet::clear(){
	//清空集合
	FD_ZERO(&_set);
	maxfd = 0;
}
void SocketSet::add(Socket *sock){
	if(sock->fd > maxfd){
		//最大的fd
		maxfd = sock->fd;
	}
	FD_SET(sock->fd,&_set);
}
bool SocketSet::is_set(Socket *sock){
	//已被设置
	return FD_ISSET(sock->fd,&_set);
}
//Select
int Socket::Select(SocketSet *r,SocketSet *w,SocketSet *e,struct timeval *t){
	fd_set *r_set = nullptr;
	fd_set *w_set = nullptr;
	fd_set *e_set = nullptr;
	//三个集合
	int maxfd = 0;
	if(r != nullptr){
		r_set = &(r->_set);
		if(r->maxfd > maxfd){
			//得到最大fd
			maxfd = r->maxfd;
		}
	}
	if(w != nullptr){
		w_set = &(w->_set);
		if(w->maxfd > maxfd){
			//得到最大fd
			maxfd = w->maxfd;
		}
	}
	if(e != nullptr){
		e_set = &(e->_set);
		if(e->maxfd > maxfd){
			//得到最大fd
			maxfd = e->maxfd;
		}
	}
	return select(maxfd+1,r_set,w_set,e_set,t);
}
