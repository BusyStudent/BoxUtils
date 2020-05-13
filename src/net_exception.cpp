#include <curl/curl.h>
#include <cstring>
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