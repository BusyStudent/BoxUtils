#include <sstream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include "net.hpp"
using namespace BoxUtils::Net;
Session *BoxUtils::Net::session = nullptr;
//会话
void BoxUtils::Net::Init(long flag){
	curl_global_init(flag);
	session = new Session();
}
void BoxUtils::Net::Quit(){
	delete session;
	curl_global_cleanup();
}
Response *BoxUtils::Net::Get(const char *url,Headers *headers,long timeout){
	//调用默认的内部的session
	return session->get(url,headers,timeout);
}
