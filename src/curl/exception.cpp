#include <curl/curl.h>
#include <exception>
#include <cstring>
#include <string>
#include "common/def.hpp"
#include "curl/curl.hpp"
#include "exception.hpp"
namespace Box{
namespace Curl{
	//异常
	EasyException::EasyException(int code):curlcode(code){
		//Easy出现了错误
	}
	//复制
	EasyException::EasyException(const EasyException &err):
		curlcode(err.curlcode){}
	EasyException::~EasyException(){

	}
	const char *EasyException::what() const throw(){
		return curl_easy_strerror(static_cast<CURLcode>(curlcode));
	}
	void EasyException::Throw(int code){
		throw EasyException(code);
	}
	HttpError::HttpError(int _code):
		code(_code),msg(std::to_string(code)){

	}
	HttpError::HttpError(const HttpError &err):
		code(err.code),msg(err.msg){

	}
	HttpError::~HttpError(){

	}
	const char *HttpError::what() const throw(){
		//输出错误
		return msg.c_str();
	}
	BOXAPI [[noreturn]] void  throwEasyException(int curlcode){
		throw EasyException(curlcode);
	}
}
}
