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
	return new Json(cJSON_CreateBool((cJSON_bool)boolen));
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
	std::ifstream stream(filename);
	//打开文件
	std::istreambuf_iterator<char> beg(stream),end;
	std::string str(beg,end);
	//读入所有字符
	Json *json = ParseString(str.c_str());//解析字符串
	stream.close();
	return json;
}
//类型判断
bool Json::is_bool(){
	return (bool)cJSON_IsBool(item);
}
bool Json::is_list(){
	//是否是列表 和数组一样
	return (bool)cJSON_IsArray(item);
}
bool Json::is_null(){
	return (bool)cJSON_IsNull(item);
}
bool Json::is_array(){
	return (bool)cJSON_IsArray(item);
}
bool Json::is_number(){
	return (bool)cJSON_IsNumber(item);
}
bool Json::is_string(){
	return (bool)cJSON_IsString(item);
}
