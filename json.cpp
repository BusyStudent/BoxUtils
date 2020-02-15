#include "cJSON.h"
#include "json.hpp"
#include "exception.hpp"
#include <cstring>
#include <string>
#include <fstream>
using namespace BoxUtils;
Json::Json(cJSON *item,bool independence){
	//用一个cJSON指针初始化
	this->item = item;
	this->independence = independence;
}
Json::Json(const Json &json){
	//复制Json
	if(json.independence == false){
		//不是独立的
		//直接复制指针
		this->item = json.item;
		this->independence = false;
		return;
	}
	else{
		this->item = cJSON_Duplicate(json.item,true);
		this->independence = true;
		return;
	}
}
Json::~Json(){
	if(independence==true){
		//如果是独立的 不是其他的成员
		cJSON_Delete(item);
	}
}
Json *Json::copy(){
	//直接复制一个
	return new Json(cJSON_Duplicate(item,cJSON_True));
}
char *Json::to_string(){
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
Json *Json::CreateArray(){
	return new Json(cJSON_CreateArray());
}
Json *Json::CreateIntArray(int *numbers,int count){
	return new Json(cJSON_CreateIntArray(numbers,count));
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
void Json::MinifyString(char *str){
	cJSON_Minify(str);
}
//类型判断
bool Json::is_bool(){
	return (bool)cJSON_IsBool(item);
}
bool Json::is_true(){
	return (bool)cJSON_IsTrue(item);
}
bool Json::is_false(){
	return (bool)cJSON_IsFalse(item);
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
//
bool Json::is_object(){
	return (bool)cJSON_IsObject(item);
}
bool Json::is_table(){
	return (bool)cJSON_IsObject(item);
}
//操作符号重载

bool Json::operator ==(const Json &json){
	//比较Json
	return (bool)cJSON_Compare(this->item,json.item,cJSON_True);
}
//设置数据和得到数据
Json Json::operator [](const char *val){
	check_is_object();
	cJSON *new_item = cJSON_GetObjectItem(item,val);
	if(new_item == nullptr){
		//没找到
		throw KeyError(val);
	}
	return Json(new_item,false);//不是独立的
}
Json Json::operator [](int val){
	//Index数组
	check_is_array();
	cJSON *new_item = cJSON_GetArrayItem(item,val);
	if(new_item == nullptr){
		throw IndexError(val);
	}
	return Json(new_item,false);
}
void Json::operator >>(int &i){
	//得到int类型数据
	if(cJSON_IsNumber(item)){
		i = item->valueint;
	}
	else{
		throw TypeError("Number",get_type_string());
	}
}
void Json::operator >>(double &number){
	//得到double
	if(cJSON_IsNumber(item)){
		number = item->valuedouble;
	}
	else{
		throw TypeError("Number",get_type_string());
	}
}
void Json::operator >>(char * & str){
	const char *raw_str;
	(*this) >> raw_str;
	//拿到原始的字符串
	str = (char*)malloc(strlen(raw_str)+1);
	strcpy(str,raw_str);//申请并且复制字符串
}
void Json::operator >>(std::string &str){
	const char *raw_str;
	(*this) >> raw_str;
	str = raw_str;
}
void Json::operator >>(const char * &str){
	//得到复制的字符串
	if(cJSON_IsString(item)){
		str = item->valuestring;
	}
	else{
		throw TypeError("String",get_type_string());
	}
}
void Json::operator >>(bool &boolen){
	//得到bool值
	if(cJSON_IsBool(item)){
		boolen = item->valueint;
	}
	else{
		throw TypeError("Bool",get_type_string());
	}
}
//类型
const char *Json::get_type_string(){
	switch(item->type & 0xFF){
		case cJSON_Number:
			return "Number";
		case cJSON_String:
			return "String";
		case cJSON_Object:
			return "Object";
		case cJSON_Array:
			return "Array";
		case cJSON_False:
			return "Bool";
		case cJSON_True:
			return "Bool";
		case cJSON_NULL:
			return "Null";
		case cJSON_Invalid:
			return "Invalid";
		case cJSON_Raw:
			return "Raw";
		default:
			return "Unknown";
	}
}
//得到一些信息
int Json::get_array_size(){
	//得到数组大小
	check_is_array();
	return cJSON_GetArraySize(item);
}
//类型检查
void Json::check_is_array(){
	if((item->type & 0xFF) != cJSON_Array){
		throw TypeError("Array",get_type_string());
	}
}
void Json::check_is_object(){
	if((item->type & 0xFF) != cJSON_Object){
		throw TypeError("Object",get_type_string());
	}
}
//加入其他节点
void Json::add_string(const char *str){
	check_is_array();//检查类型
	cJSON_AddItemToArray(item,cJSON_CreateString(str));
}
void Json::add_item(Json &item){
	//加入数组
	check_is_array();
	if(item.independence == true){
		//加入的节点是独立的
		item.independence = false;//不独立
		cJSON_AddItemToArray(this->item,item.item);//加入进去
	}
	else{
		//不是独立的 复制加入
		cJSON_AddItemToArray(this->item,cJSON_Duplicate(item.item,true));
	}
}
void Json::add_item(Json &item,const char *key){
	check_is_object();
	if(item.independence == true){
		//独立的 直接合并进去
		item.independence = false;
		cJSON_AddItemToObject(this->item,key,item.item);
	}
	else{
		//复制一下
		cJSON_AddItemToObject(this->item,key,cJSON_Duplicate(item.item,true));
	}
}
//迭代器
JsonArrayIterator Json::iter_array(){
	check_is_array();
	JsonArrayIterator iter;
	iter.item = new Json(this->item,false);
	iter.now_item = new Json(this->item->child,false);
	return iter;
}
//
JsonIterator::JsonIterator(){
	refcount = new int;
	(*refcount) = 1;
}
JsonIterator::JsonIterator(const JsonIterator &iter){
	this->refcount = iter.refcount;
	this->now_item = iter.now_item;
	this->item = iter.item;
	(*(this->refcount))++;
}
JsonIterator::~JsonIterator(){
	(*refcount) -- ;
	if(*refcount == 0){
		//没有引用了
		delete item;
		delete now_item;
		delete refcount;
	}
	
}
