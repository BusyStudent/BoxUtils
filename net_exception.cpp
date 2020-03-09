#include <string>
#include <sstream>
#include "net_exception.hpp"
using namespace Box::Net;
const char *Exception::what() const throw(){
	return reason.c_str();
}
TimeoutError::TimeoutError(std::string &hostname,int timeout){
	std::stringstream stream;
	//超时错误
	stream << "Connect "<<hostname << "Timeouted";
	reason = stream.str();
	this->timeout = timeout;
}
UnkownHostname::UnkownHostname(std::string &hostname){
	this->hostname = hostname;
	std::stringstream stream;
	stream << "UnkwonHostname: "<<hostname;
	reason = stream.str();
}
ConnectionError::ConnectionError(const char *error,int code){
	reason = error;
	this->code = code;
}
