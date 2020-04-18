#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include "exception.hpp"
#include "string.hpp"
#include "backtrace.hpp"
#ifndef BOX_NBACKTRACE
//打印BackTrace
#define SHOW_BACKTRACE() \
	Box::PrintBackTrace()
#else
#define SHOW_BACKTRACE()
#endif
Box::IndexError::IndexError(int index){
	this->index = index;
	std::stringstream stream;
	stream << "IndexError:out of range " << index;
	this->reason = stream.str();
}
const char*Box::IndexError::what()const throw(){
	SHOW_BACKTRACE();
	return reason.c_str();
}

Box::KeyError::KeyError(const char *key){
	this->key = key;
}
const char *Box::KeyError::what()const throw(){
	SHOW_BACKTRACE();
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
	SHOW_BACKTRACE();
	return reason.c_str();
}
//空指针错误
const char *Box::NullPtrException::what() const throw(){
	SHOW_BACKTRACE();
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
	
	//打印堆栈信息
	SHOW_BACKTRACE();
	abort();
}
