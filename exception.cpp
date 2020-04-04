#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include "exception.hpp"
#include "string.hpp"
#include "backtrace.hpp"
Box::IndexError::IndexError(int index){
	this->index = index;
	std::stringstream stream;
	stream << "IndexError:out of range " << index;
	this->reason = stream.str();
}
const char*Box::IndexError::what()const throw(){
	return reason.c_str();
}

Box::KeyError::KeyError(const char *key){
	this->key = key;
}
const char *Box::KeyError::what()const throw(){
	return key.c_str();
}

Box::TypeError::TypeError(const char *excepted,const char *gived){
	//类型错误
	this->excepted = excepted;
	this->gived = gived;
	std::stringstream stream;
	stream << "excepted type:" <<"'"<< excepted <<"'"<<" gived type:" <<"'"<<gived<<"'";
	this->reason = stream.str();
}
const char *Box::TypeError::what() const throw(){
	return reason.c_str();
}
//空指针错误
const char *Box::NullPtrException::what() const throw(){
	return "Got nullptr";
}
//Panic异常退出
void Box::Panic(const char *fmt,...){
	fputs("Panic():",stderr);
	va_list varg;
	va_start(varg,fmt);
	vfprintf(stderr,fmt,varg);
	va_end(varg);
	fputc('\n',stderr);
	//输出错误信息
	fputs("BackTrace:\n",stderr);
	//打印堆栈信息
	for(auto &s:Box::BackTrace()){
		std::cerr << "  by " << s << std::endl;
	}
	abort();
}
