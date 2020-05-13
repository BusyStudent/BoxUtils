#include <functional>
#include <cstring>
#include <string>
#include <fstream>
#include <utility>

#include "cJSON.h"
#include "cJSON_Utils.h"
#include "json.hpp"
#include "exception.hpp"
using namespace Box;
Json::Json(cJSON *item,bool independence){
	//用一个cJSON指针初始化
	this->item = item;
	this->independence = independence;
}
Json::Json(Json &&json){
	//移动构造函数
	if(json.independence == false){
		//不是独立的 直接复制指针
		independence = json.independence;
		item = nullptr;
	}
	else{
		//把原来的独立性改为false
		item = json.item;
		independence = json.independence;
		json.independence = false;
	}
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

Json Json::copy() const{
	//直接复制一个
	return Json(cJSON_Duplicate(item,cJSON_True));
}
char *Json::to_cstring() const{
	//字符串化
	return cJSON_Print(item);
}
std::string Json::to_string() const{
	//到C++的字符串
	char *c_str = cJSON_Print(item);
	std::string str(c_str);
	cJSON_free(c_str);
	return str;
}
void Json::print() const{
	//打印一下
	char *str = cJSON_Print(item);
	puts(str);
	cJSON_free(str);
}
//解析函数
Json Json::ParseString(const char *str){
	cJSON *item_ = cJSON_Parse(str);
	if(item_ == nullptr){
		//解析失败
		throw JsonParseError(GetError());
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
//得到错误
const char *Json::GetError(){
	return cJSON_GetErrorPtr();
}
//得到版本
const char *Json::Version(){
	return cJSON_Version();
}
//cJSON内存管理函数
void *Json::Malloc(size_t size){
	return cJSON_malloc(size);
}
void Json::Free(void *ptr){
	cJSON_free(ptr);
}
//类型判断
bool Json::is_bool() const{
	return (bool)cJSON_IsBool(item);
}
bool Json::is_true() const{
	return (bool)cJSON_IsTrue(item);
}
bool Json::is_false() const{
	return (bool)cJSON_IsFalse(item);
}
bool Json::is_list() const{
	//是否是列表 和数组一样
	return (bool)cJSON_IsArray(item);
}
bool Json::is_null() const{
	return (bool)cJSON_IsNull(item);
}
bool Json::is_array() const{
	return (bool)cJSON_IsArray(item);
}
bool Json::is_number() const{
	return (bool)cJSON_IsNumber(item);
}
bool Json::is_string() const{
	return (bool)cJSON_IsString(item);
}
//
bool Json::is_object() const{
	return (bool)cJSON_IsObject(item);
}
bool Json::is_table() const{
	return (bool)cJSON_IsObject(item);
}
//排序
void Json::sort(bool case_sensitive){
	if(case_sensitive == true){
		cJSONUtils_SortObjectCaseSensitive(item);
	}
	else{
		cJSONUtils_SortObject(item);
	}
}
//合并
void Json::merge(const Json &json,bool case_sensitive){
	if(case_sensitive == true){
		item = cJSONUtils_MergePatchCaseSensitive(item,json.item);
	}
	else{
		item = cJSONUtils_MergePatch(item,json.item);
	}
}
//操作符号重载
//比较
bool Json::cmp(const Json &json,bool case_sensitive) const{
	return (bool)cJSON_Compare(item,json.item,case_sensitive);
}
bool Json::operator ==(const Json &json) const{
	//比较Json
	return (bool)cJSON_Compare(this->item,json.item,cJSON_True);
}
//比较字符串
bool Json::operator ==(const char *str) const{
	if(is_string()){
		return strcmp(str,item->valuestring) == 0;
	}
	return false;
}
bool Json::operator ==(const std::string &str) const{
	return Json::operator ==(str.c_str());
}
//比较数字
bool Json::operator ==(int val) const{
	if(is_number()){
		return item->valueint == val;
	}
	return false;
}
bool Json::operator ==(double val) const{
	if(is_number()){
		return item->valuedouble == val;
	}
	return false;
}
//比较bool
bool Json::operator ==(bool val) const{
	if(is_bool()){
		return item->valueint == val;
	}
	return false;
}
//比较为nullptr
bool Json::operator ==(std::nullptr_t ptr) const{
	if(is_null()){
		return ptr == nullptr;
	}
	return false;
}

//设置数据和得到数据
Json Json::operator [](const char *val) const{
	check_is_object();
	cJSON *new_item = cJSON_GetObjectItem(item,val);
	if(new_item == nullptr){
		//没找到
		throw KeyError(val);
	}
	return Json(new_item,false);//不是独立的
}
Json Json::operator [](int val) const{
	//Index数组
	check_is_array();
	cJSON *new_item = cJSON_GetArrayItem(item,val);
	if(new_item == nullptr){
		throw IndexError(val);
	}
	return Json(new_item,false);
}
void Json::operator >>(int &i) const{
	//得到int类型数据
	check_is_number();
	i = item->valueint;
}
void Json::operator >>(double &number) const{
	//得到double
	check_is_number();
	number = item->valuedouble;
}
void Json::operator >>(char * & str) const{
	const char *raw_str;
	(*this) >> raw_str;
	//拿到原始的字符串
	str = (char*)malloc(strlen(raw_str)+1);
	strcpy(str,raw_str);//申请并且复制字符串
}
void Json::operator >>(std::string &str) const{
	const char *raw_str;
	(*this) >> raw_str;
	str = raw_str;
}
void Json::operator >>(const char * &str) const{
	//得到复制的字符串
	check_is_string();
	str = item->valuestring;
}
void Json::operator >>(bool &boolen) const{
	//得到bool值
	if(cJSON_IsBool(item)){
		boolen = item->valueint;
	}
	else{
		throw TypeError("Bool",get_type_string());
	}
}
//写入数据
void Json::operator <<(const int &i){
	check_is_number();
	cJSON_SetIntValue(item,i);
}
void Json::operator <<(const double &number){
	check_is_number();
	cJSON_SetNumberHelper(item,number);
}
void Json::operator <<(const std::string &str){
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
void Json::operator <<(const bool &val){
	check_is_bool();
	cJSON_SetIntValue(item,val);
}
//类型
const char *Json::get_type_string() const{
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
//是否有这个键
bool Json::has(const char *key,bool case_sensitive) const{
	check_is_object();
	if(case_sensitive == false){
		//不区分大小写
		return cJSON_HasObjectItem(item,key);
	}
	else{
		cJSON *elem;
		cJSON_ArrayForEach(elem,item){
			if(strcmp(elem->string,key) == 0){
				//找到这个键了
				return true;
			}
		}
		return false;
	}
}
int Json::size() const{
	//得到数组大小
	if(is_object() or is_array()){
		//检查是数组或者是字典
		return cJSON_GetArraySize(item);
	}
	throw TypeError("Array or Object",get_type_string());
}
int Json::get_int() const{
	//得到数字
	check_is_number();
	return item->valueint;
}
double Json::get_number() const{
	check_is_number();
	return item->valuedouble;
}
const char *Json::get_string() const{
	//得到字符串
	check_is_string();
	return item->valuestring;
}
const char *Json::get_name() const{
	//得到名字
	if(item->string != nullptr){
		return item->string;
	}
	throw NullPtrException();
}
//数值设置
void Json::set_int(const int &val){
	Json::operator <<(val);
}
void Json::set_number(const double &val){
	Json::operator <<(val);
}
void Json::set_string(const char *str){
	//设置字符串
	Json::operator <<(str);
}
void Json::set_string(const std::string &str){
	Json::operator <<(str);
}
//设置bool数值
void Json::set_bool(const bool &val){
	Json::operator <<(val);
}
//类型检查
//这里复制了一点cJSON的源代码
void Json::check_is_array() const{
	if((item->type & 0xFF) != cJSON_Array){
		throw TypeError("Array",get_type_string());
	}
}
void Json::check_is_object() const{
	if((item->type & 0xFF) != cJSON_Object){
		throw TypeError("Object",get_type_string());
	}
}
void Json::check_is_string() const{
	if((item->type & 0xFF) != cJSON_String){
		throw TypeError("String",get_type_string());
	}
}
void Json::check_is_number() const{
	if((item->type & 0xFF) != cJSON_Number){
		throw TypeError("Number",get_type_string());
	}
}
void Json::check_is_bool() const{
	if((item->type & (cJSON_True | cJSON_False)) != 0){
		throw TypeError("Bool",get_type_string());
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
//加入null数值
void Json::add_null(){
	//Array
	check_is_array();
	cJSON_AddItemToArray(item,cJSON_CreateNull());
}
void Json::add_null(const char *key){
	//Object
	check_is_object();
	cJSON_AddNullToObject(item,key);
}
//加入bool数值
void Json::add_bool(bool val){
	//Array
	check_is_array();
	cJSON_AddItemToArray(item,cJSON_CreateBool(val));
}
void Json::add_bool(const char *key,bool val){
	//Object
	check_is_object();
	cJSON_AddBoolToObject(item,key,val);
}
void Json::add_item(Json &&item){
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
void Json::add_item(Json &json){
	Json::add_item(std::move(json));
}
void Json::add_item(const char *key,Json &&item){
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
void Json::add_item(const char *key,Json &json){
	Json::add_item(key,std::move(json));
}
//迭代器
Json::TableIteration Json::iter_table(){
	check_is_object();//检查一下是否是表
	return Json::TableIteration(item);
}
Json::ArrayIteration Json::iter_array(){
	//检查是否是数组
	check_is_array();
	return Json::ArrayIteration(item);
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
//弹出
Json Json::pop(int val){
	//在数组里面
	check_is_array();
	cJSON *new_item = cJSON_DetachItemFromArray(item,val);
	//分离出来的
	if(new_item == nullptr){
		throw IndexError(val);
	}
	return Json(new_item);
}
Json Json::pop(const char *key,bool case_sensitive){
	//弹出在字典里面
	check_is_object();
	cJSON *new_item;
	if(case_sensitive == true){
		//区分大小写
		new_item = cJSON_DetachItemFromObjectCaseSensitive(item,key);
	}
	else{
		new_item = cJSON_DetachItemFromObject(item,key);
	}
	if(new_item == nullptr){
		throw KeyError(key);
	}
	return Json(new_item);
}
//特殊操作
Json *Json::clone() const{
	//克隆自己
	return new Json(cJSON_Duplicate(item,cJSON_True));
}
Json *Json::move_toheap(){
	//数据转移到堆上
	Json *j = new Json(item,independence);
	item = nullptr;
	return j;
}
//迭代器的实现
Json::Iterator Json::begin(){
	//得到开始
	if(is_object() or is_array()){
		return Json::Iterator(item->child);
	}
	else{
		throw TypeError("Array or Object",get_type_string());
	}
}
Json::Iterator Json::end(){
	//得到结束
	if(is_object() or is_array()){
		return Json::Iterator(nullptr);
	}
	else{
		throw TypeError("Array or Object",get_type_string());
	}
}
//基本迭代器
Json::Iterator::Iterator(cJSON *item){
	json = new Json(item,false);
}
Json::Iterator::Iterator(const Iterator &iter){
	//拷贝
	json = new Json(iter.json->item,false);
}
Json::Iterator::~Iterator(){
	delete json;
}
void Json::Iterator::operator ++(){
	//去往下一个
	if(json->item == nullptr){
		throw NullPtrException();
	}
	json->item = json->item->next;
}
void Json::Iterator::operator --(){
	//去往前一个
	if(json->item == nullptr){
		throw NullPtrException();
	}
	json->item = json->item->prev;
}
//迭代表
Json::TableIteration::TableIteration(cJSON *item){
	this->item = item;
}
Json::TableIter Json::TableIteration::begin(){
	//得到开始的迭代器
	return Json::TableIter(item->child);
}
Json::TableIter Json::TableIteration::end(){
	//得到结束的迭代器
	return Json::TableIter(nullptr);
}
Json::TableIter::TableIter(cJSON *item):Iterator(item){
	//不独立的Json
	//json = new Json(item,false);
}
std::string Json::TableIter::key(){
	//得到名字
	if(json->item->string != nullptr){
		return std::string(json->item->string);
	}
	throw Box::NullPtrException();
}
//数组迭代器
Json::ArrayIteration::ArrayIteration(cJSON *item){
	this->item = item;
}
Json::ArrayIter Json::ArrayIteration::begin(){
	//迭代的开始
	return Json::ArrayIter(item->child);
}
Json::ArrayIter Json::ArrayIteration::end(){
	//迭代的开始
	return Json::ArrayIter(nullptr);
}
Json::ArrayIter::ArrayIter(cJSON *item):Iterator(item){
	
}
