#include <cstring>
#include "net_exception.hpp"
using namespace Box::Net;
//异常
EasyException::EasyException(ErrType t,const char *s){
	//Easy出现了错误
	_type = t;
	_str = s;
}
const char *EasyException::what() const throw(){
	return _str;
}
ErrType EasyException::type(){
	//得到类型
	return _type;
}
