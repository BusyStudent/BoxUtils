#include <sstream>
#include <string>
#include <curl/curl.h>
#include "net.hpp"
using namespace BoxUtils::Net;
Session *BoxUtils::Net::_session = nullptr;
//会话
void BoxUtils::Net::Init(long flag){
	curl_global_init(flag);
	_session = new Session();
}
void BoxUtils::Net::Quit(){
	delete _session;
	curl_global_cleanup();
}
