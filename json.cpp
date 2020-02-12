#include "cJSON.h"
#include "json.hpp"
#include <string>
#include <fstream>
using namespace BoxUtils;
Json::Json(cJSON *item){
	//用一个cJSON指针初始化
	this->item = item;
}
Json::~Json(){
	cJSON_Delete(item);
}
const char *Json::to_string(){
	//字符串化
	return cJSON_Print(item);
}
//解析函数
Json *Json::ParseString(const char *str){
	cJSON *item_ = cJSON_Parse(str);
	if(item_ == nullptr){
		//解析失败
		return nullptr;
	}
	return new Json(item_);
}
//构造函数
Json *Json::CreateNull(){
	return new Json(cJSON_CreateNull());
}
Json *Json::CreateTrue(){
	return new Json(cJSON_CreateTrue());
}
Json *Json::CreateFalse(){
	return new Json(cJSON_CreateFalse());
}
Json *Json::CreateBool(bool boolen){
	//创建Bool
	//防止C++的bool和cjson的bool不一样
	cJSON_bool j_bool;
	if(boolen == true){
		j_bool =cJSON_True;
	}
	else{
		j_bool = cJSON_False;
	}
	return new Json(cJSON_CreateBool(j_bool));
}
Json *Json::CreateNumber(double number){
	//创建一个数字
	return new Json(cJSON_CreateNumber(number));
}
Json *Json::CreateString(const char *str){
	return new Json(cJSON_CreateString(str));
}
Json *Json::CreateStringRef(const char *str){
	return new Json(cJSON_CreateStringReference(str));
}
Json *Json::LoadFile(const char *filename){
	std::fstream stream;
	stream.open(filename,std::ios::in);
	//打开文件
	std::string str;
	stream >> str;//写入字符串
	Json *json = ParseString(str.c_str());//解析字符串
	stream.close();
	return json;
}
