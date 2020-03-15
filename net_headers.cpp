#include <cstring>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <functional>
#include "json.hpp"
#include "exception.hpp"
#include "net_headers.hpp"
#if defined(_MSC_VER) 
	#define strncasecmp _strnicmp 
#endif
#define LEN(STR) ((strchr((STR),':') - (STR))/sizeof(char))
using namespace Box::Net;
Headers::Headers(){
	slist = nullptr;
}
Headers::Headers(const Headers &h){
	slist = nullptr;
	this->update(h);
}
Headers::~Headers(){
	curl_slist_free_all((struct curl_slist*)slist);
}
void Headers::add_string(const char *str){
	//添加字符串
	slist = curl_slist_append((struct curl_slist*)slist,str);
}
void Headers::add(const char *key,const char *value){
	//加入值
	std::stringstream stream;
	stream << key << ':' << value;
	add_string(stream.str().c_str());
}
void Headers::update(const Headers &h){
	//从另一个头里加入值
	struct curl_slist *next = (struct curl_slist*)(h.slist);
	while(next != nullptr){
		//遍历链表 加入值
		add_string(next->data);
		next = next->next;
	}
}
const char *Headers::get_value(const char *key){
	//查找值
	//size_t keylen = strlen(key);//key的长度
	struct curl_slist *next = (struct curl_slist*)(slist);
	while(next != nullptr){
		if(strncasecmp(next->data,key,LEN(next->data)) == 0){
			//找到了
			return strchr(next->data,':') + 1;
		}
		next = next->next;
	}
	return nullptr;
}
const char *Headers::operator [](const char *key){
	auto value = get_value(key);
	if(value == nullptr){
		throw Box::KeyError(key);
	}
	return value;
}
bool Headers::has_key(const char *key){
	//有这个值
	auto val = get_value(key);
	if(val == nullptr){
		return false;
	}
	else{
		return true;
	}
}
bool Headers::remove(const char *key){
	bool status = false;
	//size_t keylen = strlen(key);//key的长度
	struct curl_slist *new_list = nullptr;//新的链表
	struct curl_slist *next = (struct curl_slist*)(slist);
	while(next != nullptr){
		//遍历链表 加入值
		if(strncasecmp(next->data,key,LEN(next->data)) == 0){
			//找到这个值 忽略它
			status = true;//找到
		}
		else{
			//加入数据到新的链表
			new_list = curl_slist_append(new_list,next->data);
		}
		next = next->next;
	}
	curl_slist_free_all((struct curl_slist*)(slist));
	//释放原有链表
	slist = new_list;
	return status;
}
Box::Json *Headers::json(){
	//到Json
	auto json = Box::Json::CreateTable();
	//创建一个表
	struct curl_slist *next = (struct curl_slist*)(slist);
	std::string key;
	const char *splist_mark;
	while(next != nullptr){
		//遍历链表
		splist_mark = strchr(next->data,':');//分割符号所在地点
		if(splist_mark == nullptr){
			//出错 直接忽略
			next = next->next;
			continue;
		}
		key.clear();//清空一下
		key.append(next->data,(splist_mark - (next->data))/sizeof(char));
		json->add_string(key.c_str(),  splist_mark + 1);
		next = next->next;
	}
	return json;
}
//遍历
void Headers::for_each(std::function <void(const char*,const char*)> fn){
	struct curl_slist *next = (struct curl_slist*)(slist);
	std::string key;
	const char *splist_mark;
	while(next != nullptr){
		splist_mark = strchr(next->data,':');
		key.clear();
		key.append(next->data,(splist_mark - (next->data))/sizeof(char));
		
		fn(key.c_str(),splist_mark+1);
		//调用函数
		next = next->next;
	}
}
