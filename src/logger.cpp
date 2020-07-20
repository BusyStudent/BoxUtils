#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <string>
#include <stdexcept>
#include "exception.hpp"
#include "logger.hpp"
#include "socket.hpp"
#include "string.hpp"
#include "fmt.hpp"
struct LoggerImpl{
	//记录器内部实现
	virtual ~LoggerImpl(){};
	virtual void vprintf(const char *fmt,va_list &varg) = 0;
	virtual void puts(const char *text) = 0;//写字符串
	virtual void putc(int ch) = 0;//放一个字符
	virtual void flush() = 0;//刷新
	virtual void puts_info(){
		//写出info字符串
		this->puts("INFO: ");
	};
	virtual void puts_warn(){
		this->puts("WARNING: ");
	};
	virtual void puts_error(){
		this->puts("ERROR: ");
	};
	Box::Logger::FakeStream fakestream;//虚拟流
};
static inline LoggerImpl *to_impl(void *impl){
	//转换一下
	return ((LoggerImpl*)impl);
}
//文件流实现
struct FileImpl:public LoggerImpl{
	inline FileImpl(FILE *fptr,Box::Logger *logger){
		//初始化 给个文件指针
		this->fptr = fptr;
		fakestream.logger = logger;//设置记录器指针
	}
	~FileImpl(){
		//销毁
		fclose(fptr);
	}
	void puts(const char *text){
		//打印字符串
		fputs(text,fptr);
	}
	void flush(){
		//刷新流
		fflush(fptr);
	}
	void vprintf(const char *fmt,va_list &varg){
		vfprintf(fptr,fmt,varg);
	}
	void putc(int ch){
		//打印字符
		fputc(ch,fptr);
	}
	FILE *fptr;//文件指针
};
struct StderrImpl:public LoggerImpl{
	//控制台实现
	inline StderrImpl(Box::Logger *logger){
		fakestream.logger = logger;//设置记录器指针
	}
	void puts(const char *text){
		//打印字符串
		fputs(text,stderr);
	}
	void flush(){
		//刷新流
		fflush(stderr);
	}
	void vprintf(const char *fmt,va_list &varg){
		vfprintf(stderr,fmt,varg);
	}
	void putc(int ch){
		//打印字符
		fputc(ch,stderr);
	}
	//上面基本都一样
	//不过这个是彩色字符
	void puts_error(){
		fputs("\033[31mERROR: \033[0m",stderr);
	}
	void puts_info(){
		fputs("\033[32mINFO: \033[0m",stderr);
	}
	void puts_warn(){
		fputs("\033[33mWARN: \033[0m",stderr);
	}
};
struct NetImpl:public LoggerImpl{
	//网络实现
	inline NetImpl(Box::Net::Socket *socket,Box::Logger *logger){
		this->socket = socket;
		fakestream.logger = logger;//设置记录器指针
	}
	~NetImpl(){
		delete socket;
	}
	//一些操作实现
	void flush(){
		
	}
	void puts(const char *text){
		socket->send(text,strlen(text) * sizeof(char));
	}
	void putc(int ch){
		char c = ch;
		socket->send(&c,sizeof(c));
	}
	void vprintf(const char *fmt,va_list &varg){
		//格式化输出
		auto s = Box::FmtImpl::VFormat(fmt,varg);
		socket->send(s.c_str(),s.length() * sizeof(char));
	}
	Box::Net::Socket *socket;
};
namespace Box{
	Logger::Logger(){
		impl = new StderrImpl(this);//默认输出在终端里面
	}
	Logger::~Logger(){
		delete to_impl(impl);
	}
	//设置输出在标准输出
	void Logger::set_output(){
		if(impl != nullptr){
			//销毁上次的
			delete to_impl(impl);
		}
		impl = new StderrImpl(this);
	}
	//设置文件输出
	void Logger::set_output(const std::string &output){
		FILE *fptr;//文件指针
		fptr = fopen(output.c_str(),"w");//打开
		if(fptr == nullptr){
			//失败
			throw std::runtime_error(strerror(errno));
		}
		else if(impl != nullptr){
			//销毁上次的
			delete to_impl(impl);
		}
		//实例化实现
		impl = new FileImpl(fptr,this);
	}
	void Logger::set_output(const std::string &ip,uint16_t port){
		using namespace Box::Net;
		TCP *tcp = nullptr;
		//创建一个
		try{
			if(ip.length() <= 16){
				//创建一个IPV4的Socket
				tcp = new TCP(SockFamily::IPV4);
				//连接一下
				tcp->connect(AddrV4::From(ip,port));
			}
			else{
				//创建一个IPV6的Socket
				tcp = new TCP(SockFamily::IPV6);
				//连接一下
				tcp->connect(AddrV6::From(ip,port));
			}
		}
		catch(OSError &err){
			//失败
			delete tcp;//销毁一下
			throw;
		}
		if(impl != nullptr){
			//销毁上次的
			delete to_impl(impl);
		}
		impl = new NetImpl(tcp,this);
	}
	void Logger::write(const char *text){
		//写字符串
		to_impl(impl)->puts(text);
	}
	void Logger::putc(int ch){
		to_impl(impl)->putc(ch);
	}
	void Logger::error(const char *fmt,...){
		//写错误信息
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_error();
		va_list varg;
		va_start(varg,fmt);
		//输出
		impl->vprintf(fmt,varg);
		va_end(varg);
		impl->putc('\n');//换行
	}
	void Logger::warn(const char *fmt,...){
		//写警告
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_warn();
		va_list varg;
		va_start(varg,fmt);
		//输出
		impl->vprintf(fmt,varg);
		va_end(varg);
		impl->putc('\n');//换行
	}
	void Logger::log(const char *fmt,...){
		//写详情
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_info();
		va_list varg;
		va_start(varg,fmt);
		//输出
		impl->vprintf(fmt,varg);
		va_end(varg);
		impl->putc('\n');//换行
	}
	Logger::FakeStream &Logger::error(){
		//输出错误
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_error();//先放个错误
		return impl->fakestream;
	}
	Logger::FakeStream &Logger::warn(){
		//输出警告
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_warn();
		return impl->fakestream;
	}
	Logger::FakeStream &Logger::log(){
		//输出详情
		LoggerImpl *impl = to_impl(this->impl);
		impl->puts_info();
		return impl->fakestream;
	}
};
