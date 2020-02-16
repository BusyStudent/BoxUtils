#include <curl/curl.h>
#include <sstream>
#include <cstring>
#include <vector>
#include "net.hpp"
#include "json.hpp"
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
void Headers::print(){
	curl_slist *next = slist;
	while(next!=nullptr){
		puts(next->data);
		next = next->next;
	}
}
void Headers::parse_string(const char *cstring){
	//解析字符串
	char *buf = strdup(cstring);
	char *line = buf;
	char *line_end = buf;//结尾
	while(*line!='\0'){
		line_end = strchr(line_end,'\n');
		if(line_end == nullptr){
			break;
		}
		if(*(line_end - 1)=='\r'){
			//去除'\r'
			*(line_end - 1)= '\0';
		}
		*line_end = '\0';//截断一行
		line_end ++ ;//跳过换行符号
		if(strchr(line,':') != nullptr){
			//加入链表
			slist = curl_slist_append(slist,line);
		}
		line = line_end ++;
	}
	free(buf);
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
BoxUtils::Json *Headers::to_json(){
	auto json = BoxUtils::Json::CreateTable();
	//创建一个表
	curl_slist *next = slist;
	char *buf = nullptr;
	char *cut;
	const char *vaule = nullptr;
	while(next != nullptr){
		//遍历
		buf = (char*)realloc(buf,(strlen(next->data)+1));
		strcpy(buf,next->data);
		//复制字符串
		cut = strchr(buf,':');
		//查找分割符号
		if(cut != nullptr){
			*cut = '\0';
			//切割一下
			cut++;
			while(*cut == ' '){
				//跳过空格
				cut ++;
			}
			vaule = cut;
		}
		else{
			vaule = nullptr;
		}
		json->add_string(buf,vaule);//加入字典
		next = next->next;
	}
	free(buf);
	return json;
}
void Headers::load_json(BoxUtils::Json &json){
	auto iter = json.iter_table();//遍历表
	std::stringstream stream;
	std::string key;
	std::string vaule;
	do{
		stream.clear();
		key = iter.name;
		(*iter) >> vaule;
		//导出名字和键值
		stream << key<<":"<<vaule;
		slist = curl_slist_append(slist,stream.str().c_str());
	}
	while(++iter);
}
