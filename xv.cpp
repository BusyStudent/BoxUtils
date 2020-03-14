#include "xv.hpp"
using namespace Box;
Xv::Server::Server(){
	sock = new TCPSocket();
}
Xv::Server::~Server(){
	this->clear();
	delete sock;
}
void Xv::Server::clear(){
	for(auto i = socks_vec.begin();i != socks_vec.end();++i){
		delete *i;
	}
	//清空Socket
	socks_vec.clear();
}
void Xv::Server::run(){
	SocketSet r_set;//可读入集合
	SocketSet e_set;//有错误集合
	int ret;
	std::vector <TCPSocket*> :: iterator iter;
	TCPSocket *tptr;//临时指针
	SockAddress addr;
	while(true){
		r_set.clear();
		e_set.clear();
		for(auto i = socks_vec.begin();i != socks_vec.end() ; ++ i){
			//加入所有Socket在集合里面
			r_set.add(*i);
			e_set.add(*i);
		}
		r_set.add(sock);
		//e_set.add(sock);
		
		ret = Socket::Select(&r_set,nullptr,&e_set,nullptr);
		if(ret < 0){
			//错误了
			perror("Select Error");
			return;
		}
		//Select一遍
		//是监听Socket有消息
		if(r_set.is_set(sock)){
			auto s = (sock->accept(&addr));
			//接入连接
			if(accept_user(s,addr)== false){
				//拒绝接入
				delete s;
			}
			else{
				socks_vec.push_back(s);
			}
			ret--;
		}
		//遍历vec
		iter = socks_vec.begin();
		while(ret != 0){
			if(iter == socks_vec.end()){
				//到末尾了
				break;
			}
			tptr = *iter;
			if(e_set.is_set(tptr)){
				//如果这个Socket被设置了
				ret--;
				if(HandleErr != nullptr){
					//有处理函数
					HandleErr(*this,tptr);
				}
				printf("移除Socket %p\n",tptr);
				rm_sock:;//移除Socket
				delete tptr;
				iter = socks_vec.erase(iter);
				continue;
			}
			else if(r_set.is_set(tptr)){
				//有数据可读
				ret--;
				//有数据可读
				if(recv_data(tptr) == false){
					//失败
					goto rm_sock;
				}
			}
			++iter;
		}
	}
}
bool Xv::Server::accept_user(TCPSocket*,SockAddress&){
	//默认接入所有
	return true;
}
bool Xv::Server::listen(int backlog){
	return sock->listen(backlog);
}
bool Xv::Server::bind(const char *ip,uint16_t port){
	return sock->bind(ip,port);
}
