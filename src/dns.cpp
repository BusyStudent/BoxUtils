#ifdef _WIN32
	#include <io.h>
	#include <process.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netinet/in.h> 
	#include <netdb.h>
#endif
#include <cerrno>
#include "exception.hpp"
#include "dns.hpp"
using namespace Box;
DNS::HostInfo DNS::ParseName(const char *name){
	//解析域名
	DNS::HostInfo info;
	struct hostent *ent;
	ent = gethostbyname(name);
	if(ent == nullptr){
		//失败
		return info;
	}
	if(ent->h_addrtype == AF_INET){
		info.type = Socket::Type::IPV6;
		for (int i = 0;ent->h_addr_list[i]; i++){
		//加入IPV4地址
			info.addrs.push_back(inet_ntoa(*(struct in_addr*)(ent->h_addr_list[i])));
		}
	}
	else{
		info.type = Socket::Type::IPV6;
	}
	return info;
}
DNS::HostInfo DNS::ParseName(std::string &name){
	return DNS::ParseName(name.c_str());
}
//Info实现
std::string &DNS::HostInfo::operator [](unsigned int i){
	return this->addrs[i];
}
unsigned int DNS::HostInfo::size(){
	return this->addrs.size();
}
