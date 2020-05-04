#ifndef _BOX_LINUX_CPP_
#define _BOX_LINUX_CPP_
#include <unistd.h>
#include <sys/select.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include "os.hpp"
#include "linux.hpp"
using namespace Box::Linux;
using namespace Box;
//内连函数
static inline int fd_open(const char *name,int flags){
	return open(name,flags);
}
static inline int fd_close(int fd){
	return close(fd);
}
static inline int fd_write(int fd,const void *buf,size_t bufsize){
	return write(fd,buf,bufsize);
}
static inline int fd_read(int fd,void *buf,size_t bufsize){
	return read(fd,buf,bufsize);
}
//Linux接口实现
FD::~FD(){
	//调用清理函数
	if(_cleanup_func != nullptr){
		_cleanup_func(*this);
	}
}
bool FD::open(const char *filename,int flags){
	//打开
	fd = fd_open(filename,flags);
	if(fd < 0){
		//失败
		return false;
	}
	return true;
}
bool FD::close(){
	//关闭
	if(fd_close(fd) == 0){
		return true;
	}
	return false;
}
//写
ssize_t FD::write(const void *buf,size_t bufsize){
	return fd_write(fd,buf,bufsize);
}
//读
ssize_t FD::read(void *buf,size_t bufsize){
	return fd_read(fd,buf,bufsize);
}
//写入数字
ssize_t FD::operator <<(int n){
	std::string s = std::to_string(n);
	return this->write(s.c_str(),s.length() * sizeof(char));
}
ssize_t FD::operator <<(double n){
	std::string s = std::to_string(n);
	return this->write(s.c_str(),s.length() * sizeof(char));
}
//写字符串
ssize_t FD::operator <<(const std::string &s){
	return this->write(s.c_str(),s.length() * sizeof(char));
}

//得到错误信息
const char * Linux::GetError(){
	return strerror(errno);
}
//得到错误代码
int Linux::GetErrorCode(){
	return errno;
}
#endif
