#include <curl/curl.h>
#include <string>
#include <cstring>
#include "net_headers.hpp"
#include "net_easy.hpp"
using namespace Box::Net;
Easy::Easy(){
	handle = curl_easy_init();
	#ifndef NDEBUG
	curl_easy_setopt(handle,CURLOPT_VERBOSE,1L);
	#endif
	//生成一个Handle
	#ifndef _WIN32
	curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//不要有信号
	#endif
}
Easy::Easy(const Easy &easy){
	//复制一个
	handle = curl_easy_duphandle(easy.handle);
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
	curl_easy_perform(handle);
}
void Easy::set_url(const char *url){
	//设置URL
	curl_easy_setopt(handle,CURLOPT_URL,url);
}
void Easy::set_proxy(const char *proxy){
	curl_easy_setopt(handle,CURLOPT_PROXY,proxy);
}
void Easy::set_useragent(const char *str){
	//设置User-Agent
	curl_easy_setopt(handle,CURLOPT_USERAGENT,str);
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
