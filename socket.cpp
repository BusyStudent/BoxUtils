#include "socket.hpp"
#include <cstring>
#include <stdexcept>
using namespace BoxUtils;
static inline int bind_socket(int fd,void *addr,socklen_t len){
	return bind(fd,(struct sockaddr *)addr,len);
}
static inline int close_socket(int fd){
	//关闭Socket
	return close(fd);
}
static inline int connect_socket(int fd,void *addr,socklen_t len){
	return connect(fd,(struct sockaddr*)addr,len);
}
static inline int accept_socket(int fd,void *addr,socklen_t len){
	//接受Socket
	return accept(fd,(struct sockaddr*)addr,&len);
}
static inline int listen_socket(int fd,int backlog){
	return listen(fd,backlog);
}
static inline ssize_t write_fd(int fd,const void *buf,size_t len){
	return write(fd,buf,len);
}
static inline ssize_t read_fd(int fd,void *buf,size_t len){
	return read(fd,buf,len);
}

SocketV4::Address::Address(const char *ip,unsigned int port){
	//通过地址
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	this->ip = ip;
	this->port = port;
}
void SocketV4::Address::load_struct(struct sockaddr_in &addr){
	//通过结构体
	memcpy(&(this->addr),&addr,sizeof(struct sockaddr_in));
	this->port = htons(addr.sin_port);
	this->ip = inet_ntoa(addr.sin_addr);
}
SocketV4::Socket::~Socket(){
	close_socket(fd);
}
SocketV4::Socket::Socket(int fd){
	this->fd = fd;
}
void SocketV4::Socket::bind(const char *ip,unsigned int port){
	SocketV4::Address addr(ip,port);
	bind(addr);
}
void SocketV4::Socket::bind(SocketV4::Address &addr){
	//绑定
	int ret = bind_socket(fd,&(addr.addr),sizeof(sockaddr_in));
	if(ret < 0){
		//失败
		throw std::bad_exception();
	}
}
void SocketV4::Socket::connect(SocketV4::Address &addr){
	int ret = connect_socket(fd,&(addr.addr),sizeof(sockaddr_in));
	if(ret<0){
		throw std::bad_exception();
	}
}
void SocketV4::Socket::listen(int backlog){
	if(listen_socket(fd,backlog) <0){
		throw std::bad_exception();
	}
}
void SocketV4::Socket::close(){
	close_socket(fd);
}
ssize_t SocketV4::Socket::write(const void *buf,size_t len){
	return write_fd(fd,buf,len);
}
ssize_t SocketV4::Socket::read(void *buf,size_t len){
	return read_fd(fd,buf,len);
}
ssize_t SocketV4::Socket::operator <<(std::string &str){
	return this->write(str.c_str(),str.length());
}


SocketV4::TCP::TCP(){
	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == -1){
		throw std::bad_alloc();
	}
}
SocketV4::TCP *SocketV4::TCP::accept(SocketV4::Address *addr){
	//接受连接
	struct sockaddr_in new_addr;
	int fd = accept_socket(this->fd,&new_addr,sizeof(sockaddr_in));
	if(fd < 0){
		//失败
		throw std::bad_alloc();
	}
	if(addr != nullptr){
		addr->load_struct(new_addr);
	}
	return (SocketV4::TCP*)new SocketV4::Socket(fd);
}
