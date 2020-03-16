#include <functional>
#include <cstring>
#include <string>
#include <fstream>

#include "cJSON.h"
#include "json.hpp"
#include "exception.hpp"
using namespace Box;
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
char *Json::to_cstring(){
	//字符串化
	return cJSON_Print(item);
}
std::string Json::to_string(){
	//到C++的字符串
	char *c_str = cJSON_Print(item);
	std::string str(c_str);
	free(c_str);
	return str;
}
void Json::print(){
	//打印一下
	char *str = cJSON_Print(item);
	puts(str);
	free(str);
}
//解析函数
Json Json::ParseString(const char *str){
	cJSON *item_ = cJSON_Parse(str);
	if(item_ == nullptr){
		//解析失败
		return nullptr;
	}
	return Json(item_);
}
//构造函数
Json Json::CreateNull(){
	return Json(cJSON_CreateNull());
}
Json Json::CreateTrue(){
	return Json(cJSON_CreateTrue());
}
Json Json::CreateFalse(){
	return Json(cJSON_CreateFalse());
}
Json Json::CreateBool(bool boolen){
	//创建Bool
	//防止C++的bool和cjson的bool不一样
	return Json(cJSON_CreateBool((cJSON_bool)boolen));
}
Json Json::CreateTable(){
	return Json(cJSON_CreateObject());
}
Json Json::CreateArray(){
	return Json(cJSON_CreateArray());
}
Json Json::CreateIntArray(int *numbers,int count){
	return Json(cJSON_CreateIntArray(numbers,count));
}
Json Json::CreateNumber(double number){
	//创建一个数字
	return Json(cJSON_CreateNumber(number));
}
Json Json::CreateString(const char *str){
	return Json(cJSON_CreateString(str));
}
Json Json::CreateStringRef(const char *str){
	return Json(cJSON_CreateStringReference(str));
}
Json Json::LoadFile(const char *filename){
	std::ifstream stream(filename);
	//打开文件
	std::istreambuf_iterator<char> beg(stream),end;
	std::string str(beg,end);
	//读入所有字符
	Json json = ParseString(str.c_str());//解析字符串
	stream.close();
	return json;
}
void Json::SaveFile(Json &json,const char *filename){
	//写出文件
	std::ofstream stream(filename);
	char *str = json.to_cstring();
	stream << str;
	stream.close();
	free(str);
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
	check_is_number();
	i = item->valueint;
}
void Json::operator >>(double &number){
	//得到double
	check_is_number();
	number = item->valuedouble;
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
	check_is_string();
	str = item->valuestring;
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
//写入数据
void Json::operator <<(int &i){
	check_is_number();
	cJSON_SetIntValue(item,i);
}
void Json::operator <<(double &number){
	check_is_number();
	cJSON_SetNumberHelper(item,number);
}
void Json::operator <<(std::string &str){
	//转换成C的字符串
	Json::operator <<(str.c_str());
}
void Json::operator <<(const char *str){
	check_is_string();
	if(item->valuestring != nullptr){
		//释放原有的字符串
		cJSON_free(item->valuestring);
	}
	item->valuestring = (char*)cJSON_malloc((strlen(str)+1)*sizeof(char));
	strcpy(item->valuestring,str);
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
int Json::get_int(){
	//得到数字
	check_is_number();
	return item->valueint;
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
void Json::check_is_string(){
	if((item->type & 0xFF) != cJSON_String){
		throw TypeError("String",get_type_string());
	}
}
void Json::check_is_number(){
	if((item->type & 0xFF) != cJSON_Number){
		throw TypeError("Number",get_type_string());
	}
}
//加入其他节点
void Json::add_string(const char *str){
	check_is_array();//检查类型
	cJSON_AddItemToArray(item,cJSON_CreateString(str));
}
void Json::add_string(const char *key,const char *str){
	check_is_object();
	cJSON_AddStringToObject(item,key,str);
}
void Json::add_number(double number){
	check_is_array();
	cJSON_AddItemToArray(item,cJSON_CreateNumber(number));
}
void Json::add_number(const char *key,double number){
	check_is_object();
	cJSON_AddItemToObject(item,key,cJSON_CreateNumber(number));
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
void Json::add_item(const char *key,Json &item){
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
	iter.now_item = new Json(this->item->child,false);
	return iter;
}
JsonTableIterator Json::iter_table(){
	//遍历表
	check_is_object();//检查一下
	JsonTableIterator iter;
	iter.now_item = new Json(this->item->child,false);
	iter.name = this->item->child->string;//表的名字
	return iter;
}
//内部迭代
void Json::for_array(std::function <void(Json&)> fn){
	//迭代数组
	check_is_array();
	Json json(nullptr,false);//一个不独立的Json
	cJSON * elem;
	cJSON_ArrayForEach(elem,item){
		//遍历数组
		json.item = elem;
		fn(json);
	}
}
void Json::for_table(std::function <void(const char*,Json&)> fn){
	//迭代表
	check_is_object();
	Json json(nullptr,false);
	cJSON *next = item;
	while(next != nullptr){
		json.item = next;
		fn(next->string,json);
		next = next->next;
	}
}
//特殊操作
Json *Json::clone(){
	//克隆自己
	return new Json(*this);
}
Json *Json::move_toheap(){
	//数据转移到堆上
	Json *j = new Json(item,independence);
	item = nullptr;
	return j;
}
//迭代器的实现
JsonIterator::JsonIterator(){
	refcount = new int;
	(*refcount) = 1;
}
JsonIterator::JsonIterator(const JsonIterator &iter){
	this->refcount = iter.refcount;
	this->now_item = iter.now_item;
	(*(this->refcount))++;
}
JsonIterator::~JsonIterator(){
	(*refcount) -- ;
	if(*refcount == 0){
		//没有引用了
		delete now_item;
		delete refcount;
	}
	
}
Json *JsonIterator::operator ->(){
	return now_item;
}
Json &JsonIterator::operator *(){
	return *now_item;
}
bool JsonArrayIterator::operator ++(){
	cJSON *cjson = now_item->item->next;//得到下一个
	if(cjson == nullptr){
		return false;
	}
	now_item->item = cjson;//赋值一下
	return true;
}
bool JsonTableIterator::operator ++(){
	cJSON *cjson = now_item->item->next;//得到下一个
	if(cjson == nullptr){
		return false;
	}
	now_item->item = cjson;//赋值一下
	name = cjson->string;
	return true;
}
