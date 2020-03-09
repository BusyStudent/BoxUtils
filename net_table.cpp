#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include "exception.hpp"
#include "net_table.hpp"
//VC的大小写不区分比较
#if defined(_MSC_VER) 
	#define strcasecmp _stricmp 
#endif
using namespace Box::Net;
void Headers::append(const char *key,const char *value){
	//先查找里面是否有这个值
	std::string *v = this->get(key);
	if(v != nullptr){
		*v = value;
		return;
	}
	//没找到
	_Item item;
	item.key = key;
	item.value = value;
	table.push_back(item);
}
//查找值
std::string *Headers::get(const char *key){
	for(auto i = table.begin();i != table.end();++i){
		if(strcasecmp(i->key.c_str(),key)==0){
			//比较Key一样
			return &(i->value);
		}
	}
	return nullptr;
}
//直接通过[]查找键值
std::string &Headers::operator [](const char *key){
	std::string *v = this->get(key);
	if(v != nullptr){
		return *v;
	}
	//没找到
	throw Box::KeyError(key);
}
//弹出值
std::string Headers::pop(const char *key){
	for(auto i = table.begin();i != table.end();++i){
		if(strcasecmp(i->key.c_str(),key)==0){
			//不分大小写
			std::string value = i->value;
			table.erase(i);
			return value;
		}
	}
	throw Box::KeyError(key);
}
void Headers::update(Headers &h){
	//更新一下头
	for(auto i = h.table.begin(); i!= h.table.end(); ++i){
		this->append(i->key.c_str(),i->value.c_str());
	}
}
void Headers::clear(){
	table.clear();
}
std::string Headers::to_string(){
	//格式化为字符串
	std::stringstream stream;
	for(auto i = table.begin();i!= table.end();++i){
		//遍历 加入所有值
		stream << i->key << ':'<< i->value << "\r\n";
	}
	stream << "\r\n";
	return stream.str();
}
Table &Headers::get_table(){
	return table;
}
