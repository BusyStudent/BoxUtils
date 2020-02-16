#include <cstring>
#include <string>
//默认的回调函数
#include "net.hpp"
#if defined(_MSC_VER) 
	#define strcasecmp _stricmp 
#endif
size_t BoxUtils::Net::WriteContentToMem(void *content,size_t size,size_t numb,void *param){
	size_t realsize = size * numb;//真实大小
	((std::string*)param)->append(((char*)content),0,realsize);
	return realsize;
}
size_t BoxUtils::Net::WriteHeadersToMem(void *content,size_t size,size_t numb,void *param){
	//写到字符串头部
	size_t realsize = size * numb;
	std::string str;
	str.append(((char*)content),0,realsize);
	if(strncasecmp(str.c_str(),"Set-Cookie",strlen("Set-Cookie"))!=0){
		//忽略SetCookie
		str.pop_back();
		str.pop_back();
		((BoxUtils::Net::Headers*)param)->add_string(str.c_str());
	}
	
	return realsize;
}

