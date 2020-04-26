#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
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
namespace Box{
	IndexError::IndexError(int index){
		this->index = index;
		std::stringstream stream;
		stream << "IndexError:out of range " << index;
		this->reason = stream.str();
	}
	const char*IndexError::what()const throw(){
		SHOW_BACKTRACE();
		return reason.c_str();
	}
	KeyError::KeyError(const char *key){
		this->key = key;
	}
	const char *KeyError::what()const throw(){
		SHOW_BACKTRACE();
		return key.c_str();
	}
	TypeError::TypeError(const char *excepted,const char *gived){
		//类型错误
		this->excepted = excepted;
		this->gived = gived;
		std::stringstream stream;
		stream << "excepted type:" <<"'"<< excepted <<"'"<<" gived type:" <<"'"<<gived<<"'";
		this->reason = stream.str();
	}
	const char *TypeError::what() const throw(){
		SHOW_BACKTRACE();
		return reason.c_str();
	}
	//空指针错误
	const char *NullPtrException::what() const throw(){
		SHOW_BACKTRACE();
		return "Got nullptr";
	}
	//OSError
	OSError::OSError(int code,const char *msg,const char *extra){
		if(msg == nullptr){
			//没有信息 从errno格式化
			msg = strerror(errno);
		}
		this->code = code;
		this->msg = msg;
		if(extra != nullptr){
			//检查一下是否有其他信息
			this->extra = extra;
		}
		
		//格式化自己信息
		what_msg = this->Format(code,msg,extra);
	}
	//OSError的格式化信息
	std::string OSError::Format(int code,const char *msg,const char *extra){
		std::string str;
		if(msg == nullptr){
			msg = strerror(errno);
		}
		str += ("[Errno " + std::to_string(code) +"] " + msg);
		//如果有额外信息
		if(extra != nullptr){
			str +=  ": '";
			str += extra;
			str += '\'';
		}
		return str;
	}
	const char *OSError::what() const throw(){
		SHOW_BACKTRACE();
		return what_msg.c_str();
	}
	//Panic异常退出
	void Panic(const char *fmt,...){
		fputs("Panic():",stderr);
		va_list varg;
		va_start(varg,fmt);
		vfprintf(stderr,fmt,varg);
		va_end(varg);
		fputc('\n',stderr);
		//输出错误信息
		
		//打印堆栈信息
		SHOW_BACKTRACE();
		std::terminate();
	}
};
