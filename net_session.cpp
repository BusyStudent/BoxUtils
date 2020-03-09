#include "socket.hpp"
#include "net_table.hpp"
#include "net_session.hpp"
#include "net_exception.hpp"
#include "dns.hpp"
#include <cstring>
using namespace Box::Net;
using namespace Box;
class Hook{
	//防止异常抛出后堆上内存不被释放
	public:
		~Hook(){
			delete resp;
		};
		Response *resp = nullptr;
};
Session::Session(){
	stream = false;//不保持流连接
	max_sockets = 5;
	auto table = headers.get_table();//得到表
	_Item item;
	//默认请求头
	item.key  = ("User-Agent");
	item.value = ("BoxUtils Net");
	table.push_back(item);
	//默认编码
	item.key = "Accept-Encoding";
	item.value = "gzip, deflate";
	table.push_back(item);
	//默认接受所有数据
	item.key = "Accept";
	item.value = "*/*";
	table.push_back(item);
	//默认保持链接
	item.key = "Connection";
	item.value = "keep-alive";
	table.push_back(item);
}
Session::~Session(){
	//销毁会话
	for(auto i = pool.begin();i != pool.end(); ++ i){
		//删除Socket
		delete i->second;
	}
	pool.clear();
}
Response *Session::send(Request &req){
	//发送请求
	req.headers.update(headers);
	auto addr = DNS::ParseName(req.hostname);
	SocketType::SocketType type;
	if(addr.size()==0){
		//解析失败
		throw UnkownHostname(req.hostname);
	}
	else{
		if(addr.type == AF_INET){
			type = SocketType::IPV4;
		}
		else{
			type = SocketType::IPV6;
		}
	}
	Hook hook;
	hook.resp = new Response(this,new TCPSocket(type),req.stream);
	
	auto resp = hook.resp;
	resp->sock->set_noexcept();//忽略异常
	//
	if(resp->connect(addr[0],req.port)==false){
		//连接失败
		throw ConnectionError(Socket::GetError(),req.port);
	}
	auto dat = req.format();
	if(resp->send(dat.c_str(),strlen(dat.c_str())) < 0){
		//发送数据
		throw ConnectionError(Socket::GetError(),req.port);
	}
	if(resp->stream != true){
		char buf[2560];
		size_t ret;
		
	}
	//处理完成
	hook.resp = nullptr;
	return resp;
}
