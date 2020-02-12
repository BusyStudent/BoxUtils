#include <curl/curl.h>
#include "net.hpp"
#include "net_exception.hpp"
#include <exception>
#include <stdexcept>
using namespace BoxUtils::Net;
Session::Session(int default_handles){
	//初始化放几个
	CURL *handle;
	for (int i = 0; i < default_handles; i++){
		handle = curl_easy_init();
		if(handle == nullptr){
			throw std::bad_alloc();
		}
		else{
			handles_pool.push_back(handle);
		}
	}
	write_content_func = WriteContentToMem;
	write_headers_func = WriteHeadersToMem;
}
Session::~Session(){
	this->clear_handles();
}
void Session::clear_handles(){
	std::vector <CURL *> :: iterator iter;
	for(iter = handles_pool.begin();iter != handles_pool.end();++ iter){
		curl_easy_cleanup(*iter);
	}
	handles_pool.clear();
}
void Session::add_handle(CURL *handle){
	if(handle != nullptr){
		//如果不为NULL
		curl_easy_reset(handle);//重置一下
		handles_pool.push_back(handle);
	}
	else{
		throw std::logic_error("add null handle");
	}
}
CURL *Session::allocate_handle(){
	//申请一个
	CURL *handle;
	if(handles_pool.size()==0){
		//没有了 init一个
		handle = curl_easy_init();
		if(handle == nullptr){
			throw std::bad_alloc();
		}
	}
	else{
		handle = *(--handles_pool.end());
		//得到最后一个元素
		handles_pool.pop_back();
	}
	curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//忽略信号
	return handle;
}
//Response
Response *Session::get(const char *url,Headers *u_headers,long timeout){
	CURL *handle = allocate_handle();
	//设置handle
	#ifndef NDEBUG
	//输出详情
	curl_easy_setopt(handle,CURLOPT_VERBOSE,1L);
	#endif
	if(u_headers != nullptr){
		//如果追加请求头
		Headers h = headers;
		h.update(*u_headers);
		curl_easy_setopt(handle,CURLOPT_HTTPHEADER,h.slist);
	}
	else{
		curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers.slist);//设置请求头
	}
	curl_easy_setopt(handle,CURLOPT_URL,url);//设置URL
	
	Response *resp = new Response();
	resp->handle = handle;
	resp->_session = this;//由这个对话产生
	//设置Response
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,write_content_func);//设置写出数据回调
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,&(resp->content));
	
	curl_easy_setopt(handle,CURLOPT_HEADERFUNCTION,write_headers_func);
	curl_easy_setopt(handle,CURLOPT_HEADERDATA,&(resp->headers_string));
	//设置头数据回调
	if(timeout!=0){
		//设置超时时间
		curl_easy_setopt(handle,CURLOPT_CONNECTTIMEOUT,timeout);
	}
	
	CURLcode code = curl_easy_perform(handle);
	//状态代码
	if(code!= CURLE_OK){
		ThrowExceptionByCURLcode(code,handle,url);
	}
	//得到状态代码
	curl_easy_getinfo(handle,CURLINFO_HTTP_CODE,&(resp->status_code));
	return resp;
}
