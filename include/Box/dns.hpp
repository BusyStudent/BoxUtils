#ifndef _BOX_DNS_HPP_
#define _BOX_DNS_HPP_
#include <string>
#include "socket.hpp"
#include "string.hpp"
namespace Box{
	namespace DNS{
		struct HostInfo{
			Box::Socket::Type type;//类型
			String::Vector addrs;//地址
			unsigned int size();
			std::string &operator [](unsigned int);
		};
		HostInfo ParseName(const char *name);//解析域名
		HostInfo ParseName(std::string &name);//解析域名
	};
};
#endif
