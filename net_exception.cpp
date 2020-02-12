#include <stdexcept>
#include <exception>
#include <sstream>
#include "net_exception.hpp"
using namespace BoxUtils::Net::Exception;
void BoxUtils::Net::ThrowExceptionByCURLcode(CURLcode code,CURL *handle,const char *url){
	switch(code){
		case CURLE_URL_MALFORMAT:
			//URL错误
			throw URLError(code,handle,url);
		case CURLE_COULDNT_CONNECT:
			throw ConnectionError(code,handle,url);
		default:
			throw NetError(code,handle,url);
	}
}
BaseException::BaseException(const char *reason){
	//基础的异常
	this->reason = reason;
}
const char *BaseException::what() const throw(){
	return this->reason.c_str();
}
//NetError
NetError::NetError(CURLcode _code,CURL *handle,const char *url):BaseException(curl_easy_strerror(_code)){
	//产生一个网络错误
	this->code = _code;
	this->handle = handle;
	if(url != nullptr){
		this->url = url;
	}
	std::stringstream stream;
	stream << reason <<" Code:" << code << " URL:" << url << " Handle:" << handle ;
	format_string = stream.str().c_str();
}
NetError::~NetError(){
	//回收曲柄
	curl_easy_cleanup(this->handle);
}
const char *NetError::what() const throw(){
	return format_string.c_str();
}
//URLError
URLError::URLError(CURLcode code,CURL *handle,const char *str):NetError(code,handle,str){
	
}
//ConnectionError
ConnectionError::ConnectionError(CURLcode code,CURL *handle,const char *str):NetError(code,handle,str){
	
}
