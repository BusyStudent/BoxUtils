#include <curl/curl.h>
#include <exception>
#include <cstring>
#include <string>
#include "exception.hpp"
#include "net/socket.hpp"
#include "net/exception.hpp"
namespace Box{
namespace Net{
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


	//SocketError
	SocketError::SocketError(int code):
		errcode(code),msg(OSError::Format(code,nullptr,nullptr)){
		//初始化错误
	}
	SocketError::SocketError(const SocketError &err):
		errcode(err.errcode),msg(err.msg){

	}
	SocketError::~SocketError(){}
	//输出
	const char *SocketError::what() const noexcept{
		return msg.c_str();
	}
	void SocketError::Throw(int code){
		throw SocketError(code);
	}
}
}