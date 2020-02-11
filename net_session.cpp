#include <curl/curl.h>
#include "net.hpp"
#include <exception>
#include <stdexcept>
using namespace BoxUtils::Net;
Session::Session(){
	
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
	if(handles_pool.size()==0){
		//没有了
		return curl_easy_init();
	}
	else{
		CURL *handle = *(handles_pool.end());
		//得到最后一个元素
		handles_pool.pop_back();
		return handle;
	}
}
