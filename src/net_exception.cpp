#include <curl/curl.h>
#include <cstring>
#include <string>
#include "net_exception.hpp"
using namespace Box::Net;
//异常
EasyException::EasyException(int curlcode){
	//Easy出现了错误
	_code = curlcode;
	//格式化错误
	switch(_code){
		//能分辨出来的错误代码
		case CURLE_OPERATION_TIMEDOUT:
			_type = ErrType::TIMEOUT;
			//超时
			break;
		default:
			_type = ErrType::UNKNOWN;
	}
}
const char *EasyException::what() const throw(){
	return curl_easy_strerror((CURLcode)_code);
}
void EasyException::ThrowFrom(int code){
	throw EasyException(code);
}
HttpError::HttpError(int _code){
	this->_code = _code;
}
const char *HttpError::what() const throw(){
	//输出错误
	return strdup(std::to_string(_code).c_str());
}