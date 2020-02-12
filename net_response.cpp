#include <curl/curl.h>
#include "net.hpp"
using namespace BoxUtils::Net;
Response::~Response(){
	if(_session != nullptr){
		//有所属session 放回池子
		((Session*)(_session))->add_handle(handle);
	}
	else{
		//直接释放
		curl_easy_cleanup(handle);
	}
}
