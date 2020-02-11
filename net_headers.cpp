#include <curl/curl.h>
#include <sstream>
#include <cstring>
#include "net.hpp"
using namespace BoxUtils::Net;
Headers::Headers(){
	slist = nullptr;
}
Headers::Headers(const Headers &headers){
	//复制构造器
	slist = nullptr;
	update(headers);
}
Headers::~Headers(){
	//释放单链表
	curl_slist_free_all(slist);
}
Headers *Headers::copy(){
	Headers *headers = new Headers();
	//复制自己作为备份
	headers->update(*this);
	return headers;
}
void Headers::update(const Headers & headers){
	curl_slist *next = headers.slist;
	//遍历链表 复制一下
	while(next != nullptr){
		slist = curl_slist_append(slist,next->data);
		next = next->next;
	}
}
void Headers::add_string(const char *str){
	slist = curl_slist_append(slist,str);
}
void Headers::append(const char *key,const char *val){
	std::stringstream stream;
	stream << key << ":" << val;
	add_string(stream.str().c_str());
}
void Headers::clear(){
	//清空
	curl_slist_free_all(slist);
	slist = nullptr;
}
curl_slist *Headers::get_slist(){
	return slist;
}
const char *Headers::find_value(const char *key){
	size_t keylen = strlen(key);
	curl_slist *next = slist;
	while(next != nullptr){
		if(strncmp(next->data,key,keylen)==0){
			//键值符合
			return strstr(next->data,":")+1;
			//返回vaule
		}
		next = next->next;
	}
	return nullptr;
}
const char *Headers::operator [](const char *key){
	return find_value(key);
}
