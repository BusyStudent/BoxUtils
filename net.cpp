#include <curl/curl.h>
#include "net.hpp"
bool Box::Net::Init(bool init_all){
	//初始化
	long flags;
	if(init_all){
		flags = CURL_GLOBAL_ALL;
	}
	else{
		flags = CURL_GLOBAL_DEFAULT;
	}
	auto code = curl_global_init(flags);
	if(code == CURLE_OK){
		return true;
	}
	else{
		return false;
	}
}
void Box::Net::Quit(){
	//退出
	curl_global_cleanup();
}
const char *Box::Net::Version(){
	return curl_version();
}
