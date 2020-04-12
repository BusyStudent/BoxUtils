#include <string>
#include <cstring>
#include <ctime>
#include "socket.hpp"
#include "channal.hpp"
#include "exception.hpp"
using namespace Box::Sync;
using namespace Box::Socket;
using namespace Box;
void IMP::BuildPair(void **t1,void **t2){
	//构建对
	Socket::CreatePair((BaseSocket**)t1,(BaseSocket**)t2);
	//设置不堵塞
	(*((BaseSocket**)t1))->set_nonblock(true);
	(*((BaseSocket**)t2))->set_nonblock(true);
}
void IMP::Close(void *sock){
	delete (BaseSocket*)sock;
}
void IMP::Read(void *sock,void *dat,size_t size){
	while(true){
		try{
			((BaseSocket*)sock)->read(dat,size);
			return;
		}
		catch(ReadError &err){
			if(err.code == ErrorCode::AGAIN){
				//再试一下
				WaitForRead(sock);
				continue;
			}
			throw;
		}
	}
}
void IMP::Write(void *sock,const void *dat,size_t size){
	((BaseSocket*)sock)->write(dat,size);
}
void IMP::Flush(void *sock){
	((BaseSocket*)sock)->flush();
}
//读入一行
void IMP::ReadString(void *sock,std::string &s){
	BaseSocket &socket = (*((BaseSocket*)sock));//Socket
	WaitForRead(sock);//等待可以读入
	size_t len;
	socket.read(&len,sizeof(size_t));
	size_t byteread;//已经读入大小
	size_t size;//需要读入大小
	char buf[64];
	while(len > 0){
		if(len > (int)sizeof(buf)){
			//需要读入长度大于缓冲区
			size = sizeof(buf);
		}
		else{
			size = len;
		}
		byteread = socket.read(buf,size);
		s.append(buf,byteread);
		len -= byteread;
	}
}
//写出
void IMP::WriteString(void *sock,const std::string &s){
	BaseSocket &socket = (*((BaseSocket*)sock));//Socket
	size_t len = s.length();//字符串长度
	socket.write(&len,sizeof(size_t));
	socket.write(s.c_str(),len);
}
//在时间内可读
bool IMP::ReadAble(void *sock,int ms){
	BaseSocket &s = *((BaseSocket*)sock);
	int ch;//字符
	ch = s.getc();
	if(ch != EOF){
		//没失败
		s.ungetc(ch);//放回
		return true;
	}
	//尝试select
	Set sset;
	sset.add(s);
	//加入Socket
	timeval t={
		0,
		ms * 100
	};
	auto ret = Select(&sset,nullptr,nullptr,&t);
	if(ret <= 0){
		return false;
	}
	if(sset.is_set(s)){
		//被设置了
		return true;
	}
	return false;
}
void IMP::WaitForRead(void *sock){
	//等待可读
	BaseSocket &s = *((BaseSocket*)sock);
	int ch;//字符
	ch = s.getc();
	//尝试得到字符
	if(ch != EOF){
		//放回去
		s.ungetc(ch);
		return;
	}
	Set sset;//加入集合
	sset.add(s);
	Select(&sset,nullptr,nullptr,nullptr);
}
