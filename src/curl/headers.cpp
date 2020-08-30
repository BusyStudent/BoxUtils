#include <cstring>
#include <string>
#include <sstream>
#include <string_view>
#include <curl/curl.h>
#include <functional>
#include <initializer_list>
#include "common/def.hpp"
#include "json.hpp"
#include "fmt.hpp"
#include "exception.hpp"
#include "curl/curl.hpp"
#if defined(_MSC_VER) 
	#define strncasecmp _strnicmp 
#elif defined(_WIN32)
	//Window下msys2没有这个strncasecmp
	#include "libc/mem.h"
	#define strncasecmp Box_strncasecmp
#endif
#define LEN(STR) ((strchr((STR),':') - (STR))/sizeof(char))
namespace Box{
namespace Curl{
	//通过Json格式化
	Headers::Headers(JsonRef ref){
		slist = nullptr;
		for(auto &item:ref){
			add(item.name(),item.get_value<std::string_view>());
		}
	}
	Headers::Headers(std::initializer_list<std::string_view> vlist){
		for(auto &s:vlist){
			//添加字符串
			add_string(s);
		}
	}
	Headers::Headers(std::initializer_list<std::initializer_list<std::string_view>> vlist){
		for(auto &l:vlist){
			//add(l.begin(),--l.end());
			if(l.size() != 2){
				//如果没有两个元素
				throw InvalidArgument(nullptr);
			}
			auto begin = l.begin();
			++ begin;
			add(*(l.begin()),*begin);
		}
	}
	Headers::Headers(Headers &&headers){
		//移动构造器
		slist = headers.slist;
		headers.slist = nullptr;
	}
	Headers::Headers(const Headers &h){
		slist = nullptr;
		this->update(h);
	}
	Headers::~Headers(){
		curl_slist_free_all((struct curl_slist*)slist);
	}
	void Headers::add_string(std::string_view str){
		//添加字符串
		slist = curl_slist_append((struct curl_slist*)slist,str.data());
	}
	void Headers::add(std::string_view key,std::string_view value){
		//加入值
		add_string(Format("{}:{}",key,value));
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
	std::string_view Headers::get_value(std::string_view key) const{
		//查找值
		//size_t keylen = strlen(key);//key的长度
		struct curl_slist *next = (struct curl_slist*)(slist);
		while(next != nullptr){
			if(strncasecmp(next->data,key.data(),LEN(next->data)) == 0){
				//找到了
				return strchr(next->data,':') + 1;
			}
			next = next->next;
		}
		return std::string_view();//返回一个空的
	}
	std::string_view Headers::operator [](std::string_view key) const{
		auto value = get_value(key);
		if(value == nullptr){
			throwKeyError(key);
		}
		return value;
	}
	bool Headers::has_key(std::string_view key) const{
		//有这个值
		auto val = get_value(key);
		if(val == nullptr){
			return false;
		}
		else{
			return true;
		}
	}
	bool Headers::remove(std::string_view key){
		bool status = false;
		//size_t keylen = strlen(key);//key的长度
		struct curl_slist *new_list = nullptr;//新的链表
		struct curl_slist *next = (struct curl_slist*)(slist);
		while(next != nullptr){
			//遍历链表 加入值
			if(strncasecmp(next->data,key.data(),LEN(next->data)) == 0){
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
	Box::Json Headers::json(){
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
			json.insert(key.c_str(),  splist_mark + 1);
			next = next->next;
		}
		return json;
	}
	//遍历
	void Headers::for_each(const std::function <void(const char*,const char*)> &fn) const{
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
}
}
