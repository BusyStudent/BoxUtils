#include <new>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include "common/def.hpp"
#include "exception.hpp"
#include "string.hpp"
#include "backtrace.hpp"
#include "fmt.hpp"
#ifndef NDEBUG
	//调试Backtrace
	#define BOX_DBG_BACKTRACE() do{\
		fprintf(stderr,"Error at %s\n",__FUNCTION__);\
		Box::Backtrace::Show();\
		fflush(stderr);\
	}\
	while(0);
#else
	#define BOX_DBG_BACKTRACE() 
#endif
namespace Box{
	IndexError::IndexError(int index):
		index(index),
		reason(Format("IndexError:out of range {}",index)){
		//格式化一下
	}
	const char*IndexError::what()const throw(){
		return reason.c_str();
	}
	//NotFoundError
	NotFoundError::NotFoundError(std::string_view str):
		data(str){

	}
	NotFoundError::NotFoundError(const NotFoundError &err):
		data(err.data){

	}
	NotFoundError::~NotFoundError(){}
	const char *NotFoundError::what() const noexcept{
		return data.c_str();
	}
	//KeyError
	KeyError::KeyError(std::string_view key):
		NotFoundError(key){
		
	}
	KeyError::KeyError(const KeyError &err)
		:NotFoundError(err){

	}
	KeyError::~KeyError(){

	}
	TypeError::TypeError(std::string_view expected,std::string_view gived):
		expected(expected),
		gived(gived),
		reason(Format("TypeError:({},{})",expected,gived)){

	}
	const char *TypeError::what() const throw(){
		return reason.c_str();
	}
	//空指针错误
	const char *NullPtrException::what() const throw(){
		return msg.c_str();
	}
	//构造和其他函数
	NullPtrException::NullPtrException(std::string_view str):
		msg(Format("NullPtrException:{}",str)){
	}
	NullPtrException::NullPtrException():msg("Got nullptr"){}
	NullPtrException::NullPtrException(const NullPtrException&){}
	NullPtrException::~NullPtrException(){}
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
		return what_msg.c_str();
	}
	//Panic异常退出
	BOXAPI void Panic(const char *fmt,...){
		fputs("Panic():",stderr);
		va_list varg;
		va_start(varg,fmt);
		vfprintf(stderr,fmt,varg);
		va_end(varg);
		fputc('\n',stderr);
		//输出错误信息
		
		//打印堆栈信息
		Backtrace::Show();
		std::terminate();
	}
	//FileNotFoundErrpr
	FileNotFoundError::FileNotFoundError(int code,const char *filename)
		:OSError(code,nullptr,filename),fname(filename){
	}
	FileNotFoundError::FileNotFoundError(const FileNotFoundError &err)
		:OSError(err),fname(err.fname){

	}
	FileNotFoundError::~FileNotFoundError(){

	}
	//JsonParseError
	JsonParseError::JsonParseError(const char *msg){
		this->msg = msg;
	}
	JsonParseError::JsonParseError(const JsonParseError &err)
		:msg(err.msg){

	}
	JsonParseError::~JsonParseError(){}
	const char *JsonParseError::what() const throw(){
		return msg.c_str();
	}
	//抛出异常函数
	BOXAPI [[noreturn]] void throwNullPtrException(){
		BOX_DBG_BACKTRACE();
		throw NullPtrException();
	};
	BOXAPI [[noreturn]] void throwNullPtrException(std::string_view info){
		BOX_DBG_BACKTRACE();
		throw NullPtrException(info);
	};
	BOXAPI [[noreturn]] void throwNotFoundError(std::string_view info){
		BOX_DBG_BACKTRACE();
		throw NotFoundError(info);
	};
	BOXAPI [[noreturn]] void throwKeyError(std::string_view key){
		BOX_DBG_BACKTRACE();
		throw KeyError(key);
	};
	BOXAPI [[noreturn]] void throwTypeError(std::string_view expected,std::string_view gived){
		BOX_DBG_BACKTRACE();
		throw TypeError(expected,gived);
	}
	BOXAPI [[noreturn]] void throwBadAlloc(){
		BOX_DBG_BACKTRACE();
		throw std::bad_alloc();
	};
	BOXAPI [[noreturn]] void throwIndexError(int index){
		BOX_DBG_BACKTRACE();
		throw IndexError(index);;
	};
};
