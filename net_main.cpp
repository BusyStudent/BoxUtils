#include <cstring>
#include <string>
#include <sstream>
#include <stdexcept>
#include "net_exception.hpp"
#include "net_main.hpp"
#include "socket.hpp"
#if defined(_MSC_VER) 
	#define strcasecmp _stricmp 
#endif
using namespace Box::Net;
Request::Request(Method::T method,const char *curl,
	Headers *headers,bool stream){
	this->method = method;
	this->stream = stream;
	
	if(headers != nullptr){
		//加入请求的头部
		auto table = this->headers.get_table();
		auto v = headers->get_table();
		for(auto i = v.begin();i!= v.end(); ++i){
			table.push_back(*i);
		}
	}
	std::string url(curl);
	//改为C++的String
	auto pos = url.find("://");
	if(pos == url.npos){
		//没找到
		throw std::invalid_argument("非法URL");
	}
	else{
		//把开始部分加入
		schema.append(url.substr(0,pos));
		//设置端口
		const char *s = schema.c_str();
		if(strcasecmp(s,"http")==0){
			//HTTP
			port = 80;
		}
		else if(strcasecmp(s,"https")==0){
			port = 443;
		}
		else{
			throw std::invalid_argument("非法URL");
		}
	}
	auto _pos = url.find('/',pos+strlen("://"));
	if(_pos != url.npos){
		//找到
		auto new_pos = pos+strlen("://");
		std::string server(url,new_pos,_pos-new_pos);
		//找到服务器名字
		auto port_pos = server.find(':');
		if(port_pos != server.npos){
			//找到端口
			hostname.append(server.substr(0,port_pos));
			port = std::atoi(&(server.c_str()[port_pos+1]));
		}
		else{
			hostname.append(server);
		}
		path.append(&url[_pos]);
	}
	else{
		hostname.append(url,pos+strlen("://"));
		path = '/';
	}
}
std::string Request::format(){
	std::stringstream stream;
	const char *m;
	switch(method){
		case Method::POST:
			m = "POST";
			break;
		case Method::GET:
			m = "GET";
			break;
		default:
			abort();
	}
	stream << m << " " << path << " HTTP/1.1\r\n";
	stream << headers.to_string();
	return stream.str();
}
//Response
Response::Response(Session *session,TCPSocket *sock,bool stream){
	this->session = session;
	this->sock = sock;
	this->stream = stream;
}
Response::~Response(){
	if(session == nullptr){
		//如果没有会话的画
		delete sock;
	}
}
//函数指针
ssize_t Response::send(const void *buf,size_t len){
	return this->send_data_func(*this,buf,len);
}
ssize_t Response::recv(void *buf,size_t len){
	return this->recv_data_func(*this,buf,len);
}
bool Response::connect(std::string &ip,uint16_t port){
	return this->connect_func(*this,ip,port);
}
//发送和读取数据
ssize_t Response::HTTPSendData(Response &resp,const void *buf,size_t len){
	return resp.sock->write(buf,len);
}
ssize_t Response::HTTPRecvData(Response &resp,void *buf,size_t len){
	if(resp.timeout > 0){
		//有超时设置
		struct timeval t= {
			resp.timeout,
			0
		};
		SocketSet sset;
		sset.add(resp.sock);
		auto ret = Socket::Select(&sset,nullptr,nullptr,&t);
		//等待事件
		if(ret == 0){
			//超时
			throw TimeoutError(resp.hostname,resp.timeout);
		}
		else if(ret < 0){
			//直接Socket错误了
			throw ConnectionError(Socket::GetError(),errno);
		}
	}
	return resp.sock->read(buf,len);
}
