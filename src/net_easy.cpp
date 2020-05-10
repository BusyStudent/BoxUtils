#include <curl/curl.h>
#include <string>
#include <cstring>
#include "exception.hpp"
#include "net_exception.hpp"
#include "net_headers.hpp"
#include "net_easy.hpp"
using namespace Box::Net;
Easy::Easy(void *hd){
	handle = hd;
}
Easy::Easy(){
	handle = curl_easy_init();
	//设置为自己本身
	curl_easy_setopt(handle,CURLOPT_PRIVATE,this);
	#ifndef NDEBUG
	curl_easy_setopt(handle,CURLOPT_VERBOSE,1L);
	#endif
	//生成一个Handle
	#ifndef _WIN32
	curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//不要有信号
	#endif
	//清空本身数据
	multi_userdata = nullptr;
}
Easy::Easy(const Easy &easy){
	//复制一个
	handle = curl_easy_duphandle(easy.handle);
	curl_easy_setopt(handle,CURLOPT_PRIVATE,this);
	multi_userdata = nullptr;
}
Easy::~Easy(){
	curl_easy_cleanup(handle);
}
void Easy::reset(){
	curl_easy_reset(handle);
	#ifndef _WIN32
	curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//不要有信号
	#endif
}
void Easy::perform(){
	//进行传输
	auto code = curl_easy_perform(handle);
	if(code != CURLE_OK){
		ErrType type;//类型
		switch(code){
			//能分辨出来的错误代码
			case CURLE_OPERATION_TIMEDOUT:
				type = ErrType::TIMEOUT;
				//超时
				break;
			default:
				type = ErrType::UNKNOWN;
		}
		throw EasyException(type,curl_easy_strerror(code));
	}
}

//启动cookie引擎
void Easy::enable_cookie(){
	curl_easy_setopt(handle,CURLOPT_COOKIEFILE,"");
}

void Easy::set_url(const char *url){
	//设置URL
	curl_easy_setopt(handle,CURLOPT_URL,url);
}
void Easy::set_method(Method method){
	//设置方法
	switch(method){
		case Method::POST:{
			curl_easy_setopt(handle,CURLOPT_POST,1L);
			break;
		}
		case Method::GET:{
			curl_easy_setopt(handle,CURLOPT_HTTPGET,1L);
			break;
		}
		case Method::PUT:{
			curl_easy_setopt(handle,CURLOPT_PUT,1L);
			break;
		}
	}
}
void Easy::set_timeout(long timeout){
	//设置超时
	curl_easy_setopt(handle,CURLOPT_CONNECTTIMEOUT,&timeout);
}
void Easy::set_proxy(const char *proxy){
	curl_easy_setopt(handle,CURLOPT_PROXY,proxy);
}
void Easy::set_useragent(const char *str){
	//设置User-Agent
	curl_easy_setopt(handle,CURLOPT_USERAGENT,str);
}
void Easy::set_referer(const char *str){
	//设置referer
	curl_easy_setopt(handle,CURLOPT_REFERER,str);
}
void Easy::set_headers(){
	//重置会原有的请求头
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,nullptr);
}
void Easy::set_headers(const Headers & h){
	//使用请求头替换里面的头部
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,h.slist);
}
void Easy::set_ostream(std::string &str){
	//设置输出字符串
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,WriteToString);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,&str);
}
void Easy::set_ostream(FILE *f){
	//设置输出流
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,WriteToFILE);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,f);
}
void Easy::set_oheaders(Headers &h){
	//设置输出的响应头
	curl_easy_setopt(handle,CURLOPT_HEADERFUNCTION,WriteToHeaders);
	curl_easy_setopt(handle,CURLOPT_HEADERDATA,&h);
}
void Easy::set_followlocation(){
	//自动更寻重定向
	curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(handle,CURLOPT_AUTOREFERER,1L);
}
void Easy::set_write_cb(EasyCallBack cb,void *param){
	//写的CALLBACK
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,cb);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,param);
}
void Easy::set_header_cb(EasyCallBack cb,void *param){
	//头的callback
	curl_easy_setopt(handle,CURLOPT_HEADERFUNCTION,cb);
	curl_easy_setopt(handle,CURLOPT_HEADERDATA,param);
}

void *Easy::get_handle() const{
	//得到CURL的Handle
	return handle;
}
long Easy::status_code()const{
	//得到状态代码
	long code;
	curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,&code);
	return code;
}
std::string Easy::url() const{
	//得到URL
	const char *url;
	curl_easy_getinfo(handle,CURLINFO_EFFECTIVE_URL,&url);
	return std::string(url);
}
bool Easy::ok()const{
	if(status_code() == 200){
		return true;
	}
	else{
		return false;
	}
}
//克隆
Easy *Easy::clone()const{
	return new Easy(curl_easy_duphandle(handle));
}
//URL编码和解码

std::string Easy::escape_url(const char *url) const{
	char *s = curl_easy_escape(handle,url,0);
	//编码一下
	if(s == nullptr){
		throw Box::NullPtrException();
	}
	//失败
	std::string str(s);
	curl_free(s);
	return str;
}
std::string Easy::unescape_url(const char *url) const{
	//解码URL
	char *s = curl_easy_escape(handle,url,0);
	if(s == nullptr){
		throw Box::NullPtrException();
	}
	std::string str(s);
	curl_free(s);
	return str;
}

//回调函数
size_t Easy::WriteToFILE(char *buf,size_t size,size_t block,void *param){
	//写到文件
	return fwrite(buf,size,block,(FILE*)param);
}
size_t Easy::WriteToString(char *buf,size_t size,size_t block,void *param){
	//写到字符串里
	size_t s = size*block;
	((std::string*)param)->append(buf,s);
	return s;
}
size_t Easy::WriteToHeaders(char *buf,size_t size,size_t block,void *param){
	std::string strbuf(buf,block);
	if(strbuf.find(':') == strbuf.npos){
		//没找到:
		//跳过
		return size * block;
	}
	#if 1
	//这里处理末尾的\r\n
	std::string::iterator iter = --strbuf.end();
	while(*iter == '\r' or *iter == '\n'){
		//取出\r\n
		strbuf.pop_back();
		iter = --strbuf.end();
	}
	#endif
	//直接加入
	//strbuf.replace(strbuf.begin(),strbuf.end(),"\r\n","");
	((Headers*)param)->add_string(strbuf.c_str());
	return size*block;
}
//得到引用
Easy &Easy::GetRefFrom(void *handle){
	Easy *ref = nullptr;
	curl_easy_getinfo(handle,CURLINFO_PRIVATE,&ref);
	if(ref == nullptr){
		throw Box::NullPtrException();
	}
	return *ref;
}