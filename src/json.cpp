#include <functional>
#include <cstring>
#include <string>
#include <fstream>
#include <ostream>
#include <utility>
#include <mutex>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "json.hpp"
#include "exception.hpp"
namespace Box{
	JsonHolder::~JsonHolder(){
		if(not ref){
			cJSON_Delete(cjson);
		}
	};
	inline JsonHolder::operator cJSON*() const noexcept{
		return cjson;
	};
	inline const cJSON *JsonHolder::get() const noexcept{
		return cjson;
	};
	inline cJSON *JsonHolder::get() noexcept{
		return cjson;
	};
	//创建一个Holder
	inline JsonHolder* CreateJsonHolder(cJSON *cjson,bool ref){
		return new JsonHolder{
			.cjson = cjson,
			.ref = ref
		};
	};

Json::Json(cJSON *cjson,bool ref):
	holder(CreateJsonHolder(cjson,ref)){
	//用一个cJSON指针初始化
}
Json::Json(Json &&json)
	:holder(json.holder){
	//移动构造函数
}
Json::Json(const Json &json)
	:holder(json.holder){
	//浅拷贝Json
}
Json::~Json(){

}
//从初始化列表构建
Json::Json(const std::initializer_list<int> &vlist)
	:holder(CreateJsonHolder(cJSON_CreateArray(),false)){
	//创建一个int数组
	cJSON *cjson = *holder;
	for(auto &val:vlist){
		cJSON_AddItemToArray(cjson,cJSON_CreateNumber(val));
	}
}
Json::Json(const std::initializer_list<double> &vlist)
	:holder(CreateJsonHolder(cJSON_CreateArray(),false)){
	//创建double数组
	cJSON *cjson = *holder;
	for(auto &val:vlist){
		cJSON_AddItemToArray(cjson,cJSON_CreateNumber(val));
	}
}
Json::Json(const std::initializer_list<const char*> &vlist)
	:holder(CreateJsonHolder(cJSON_CreateArray(),false)){
	//创建String
	cJSON *cjson = *holder;
	for(auto &val:vlist){
		cJSON_AddItemToArray(cjson,cJSON_CreateString(val));
	}
}

Json Json::copy() const{
	//直接复制一个
	return Json(cJSON_Duplicate((*holder),true));
}
char *Json::to_cstring() const{
	//字符串化
	return cJSON_Print((*holder));
}
std::string Json::to_string() const{
	//到C++的字符串
	char *c_str = cJSON_Print((*holder));
	std::string str(c_str);
	cJSON_free(c_str);
	return str;
}
void Json::print() const{
	//打印一下
	char *str = cJSON_Print((*holder));
	puts(str);
	cJSON_free(str);
}
//解析函数
Json Json::ParseString(const char *str){
	#ifdef __STDC_NO_THREADS__
	//线程不安全 加锁
	static std::mutex mtx;
	std::lock_guard<std::mutex> locker(mtx);
	#endif
	cJSON *cjson = cJSON_Parse(str);
	if(cjson == nullptr){
		//解析失败
		throw JsonParseError(GetError());
	}
	return Json(cjson);
}
Json Json::ParseString(const std::string &str){
	return Json::ParseString(str.c_str());
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
	if(stream.bad()){
		//失败
		throw FileNotFoundError(errno,filename);
	}
	//读入所有字符
	try{
		Json json = ParseString(str.c_str());//解析字符串
		stream.close();
		return json;
	}
	catch(JsonParseError&){
		//关闭一下流
		stream.close();
		throw;
	};
}
Json Json::ParseFrom(std::istream &stream){
	//从流解析
	std::istreambuf_iterator<char> beg(stream),end;
	std::string str(beg,end);
	return ParseString(str);
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
	return (bool)cJSON_IsBool((*holder));
}
bool Json::is_true() const{
	return (bool)cJSON_IsTrue((*holder));
}
bool Json::is_false() const{
	return (bool)cJSON_IsFalse((*holder));
}
bool Json::is_list() const{
	//是否是列表 和数组一样
	return (bool)cJSON_IsArray((*holder));
}
bool Json::is_null() const{
	return (bool)cJSON_IsNull((*holder));
}
bool Json::is_array() const{
	return (bool)cJSON_IsArray((*holder));
}
bool Json::is_number() const{
	return (bool)cJSON_IsNumber((*holder));
}
bool Json::is_string() const{
	return (bool)cJSON_IsString((*holder));
}
//
bool Json::is_object() const{
	return (bool)cJSON_IsObject((*holder));
}
bool Json::is_table() const{
	return (bool)cJSON_IsObject((*holder));
}
//排序
void Json::sort(bool case_sensitive){
	if(case_sensitive == true){
		cJSONUtils_SortObjectCaseSensitive((*holder));
	}
	else{
		cJSONUtils_SortObject((*holder));
	}
}
//合并
void Json::merge(const Json &json,bool case_sensitive){
	if(case_sensitive == true){
		holder->cjson = cJSONUtils_MergePatchCaseSensitive((*holder),*json.holder);
	}
	else{
		holder->cjson = cJSONUtils_MergePatch((*holder),*json.holder);
	}
}
//操作符号重载
//比较
bool Json::cmp(const Json &json,bool case_sensitive) const{
	return (bool)cJSON_Compare((*holder),*json.holder,case_sensitive);
}
bool Json::operator ==(const Json &json) const{
	//比较Json
	return (bool)cJSON_Compare(*holder,*json.holder,true);
}
//比较字符串
bool Json::operator ==(const char *str) const{
	if(is_string()){
		return strcmp(str,holder->get()->valuestring) == 0;
	}
	return false;
}
bool Json::operator ==(const std::string &str) const{
	return Json::operator ==(str.c_str());
}
//比较数字
bool Json::operator ==(int val) const{
	if(is_number()){
		return holder->get()->valueint == val;
	}
	return false;
}
bool Json::operator ==(double val) const{
	if(is_number()){
		return holder->get()->valuedouble == val;
	}
	return false;
}
//比较bool
bool Json::operator ==(bool val) const{
	if(is_bool()){
		return holder->get()->valueint == val;
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
	cJSON *cjson = cJSON_GetObjectItem(*holder,val);
	if(cjson == nullptr){
		//没找到
		throw KeyError(val);
	}
	return Json(cjson,true);//是引用
}
Json Json::operator [](int val) const{
	//Index数组
	check_is_array();
	cJSON *cjson = cJSON_GetArrayItem(*holder,val);
	if(cjson == nullptr){
		throw IndexError(val);
	}
	return Json(cjson,true);
}
void Json::operator >>(int &i) const{
	//得到int类型数据
	check_is_number();
	i = holder->get()->valueint;
}
void Json::operator >>(double &number) const{
	//得到double
	check_is_number();
	number = holder->get()->valuedouble;
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
	str = holder->get()->valuestring;
}
void Json::operator >>(bool &boolen) const{
	//得到bool值
	if(cJSON_IsBool((*holder))){
		boolen = holder->get()->valueint;
	}
	else{
		throw TypeError("Bool",get_type_string());
	}
}
//写入数据
void Json::operator <<(const int &i){
	check_is_number();
	cJSON *cjson = *holder;
	cJSON_SetIntValue(cjson,i);
}
void Json::operator <<(const double &number){
	check_is_number();
	cJSON_SetNumberHelper((*holder),number);
}
void Json::operator <<(const std::string &str){
	//转换成C的字符串
	Json::operator <<(str.c_str());
}
void Json::operator <<(const char *str){
	check_is_string();
	struct cJSON *cjson = *holder;//得到cjson结构体
	if(cjson->valuestring != nullptr){
		//释放原有的字符串
		cJSON_free(cjson->valuestring);
	}
	cjson->valuestring = (char*)cJSON_malloc((strlen(str)+1)*sizeof(char));
	strcpy(cjson->valuestring,str);
}
void Json::operator <<(const bool &val){
	check_is_bool();
	struct cJSON *cjson = *holder;
	cJSON_SetIntValue(cjson,val);
}
//类型
const char *Json::get_type_string() const{
	switch(holder->get()->type & 0xFF){
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
		return cJSON_HasObjectItem(*holder,key);
	}
	else{
		cJSON *elem;
		cJSON *cjson = (*holder);
		cJSON_ArrayForEach(elem,cjson){
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
		return cJSON_GetArraySize((*holder));
	}
	throw TypeError("Array or Object",get_type_string());
}
int Json::get_int() const{
	//得到数字
	check_is_number();
	return holder->get()->valueint;
}
double Json::get_number() const{
	check_is_number();
	return holder->get()->valuedouble;
}
const char *Json::get_string() const{
	//得到字符串
	check_is_string();
	return holder->get()->valuestring;
}
const char *Json::get_name() const{
	//得到名字
	if(holder->get()->string != nullptr){
		return holder->get()->string;
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
	if((holder->get()->type & 0xFF) != cJSON_Array){
		throw TypeError("Array",get_type_string());
	}
}
void Json::check_is_object() const{
	if((holder->get()->type & 0xFF) != cJSON_Object){
		throw TypeError("Object",get_type_string());
	}
}
void Json::check_is_string() const{
	if((holder->get()->type & 0xFF) != cJSON_String){
		throw TypeError("String",get_type_string());
	}
}
void Json::check_is_number() const{
	if((holder->get()->type & 0xFF) != cJSON_Number){
		throw TypeError("Number",get_type_string());
	}
}
void Json::check_is_bool() const{
	if((holder->get()->type & (cJSON_True | cJSON_False)) != 0){
		throw TypeError("Bool",get_type_string());
	}
}
//加入其他节点
void Json::add_string(const char *str){
	check_is_array();//检查类型
	cJSON_AddItemToArray((*holder),cJSON_CreateString(str));
}
void Json::add_string(const char *key,const char *str){
	check_is_object();
	cJSON_AddStringToObject((*holder),key,str);
}
void Json::add_number(double number){
	check_is_array();
	cJSON_AddItemToArray((*holder),cJSON_CreateNumber(number));
}
void Json::add_number(const char *key,double number){
	check_is_object();
	cJSON_AddItemToObject((*holder),key,cJSON_CreateNumber(number));
}
//加入null数值
void Json::add_null(){
	//Array
	check_is_array();
	cJSON_AddItemToArray((*holder),cJSON_CreateNull());
}
void Json::add_null(const char *key){
	//Object
	check_is_object();
	cJSON_AddNullToObject((*holder),key);
}
//加入bool数值
void Json::add_bool(bool val){
	//Array
	check_is_array();
	cJSON_AddItemToArray((*holder),cJSON_CreateBool(val));
}
void Json::add_bool(const char *key,bool val){
	//Object
	check_is_object();
	cJSON_AddBoolToObject((*holder),key,val);
}
void Json::add_item(Json &&json){
	//加入数组
	check_is_array();
	if(not holder->ref){
		//加入的节点不是引用
		holder->ref = true;//改为引用
		cJSON_AddItemToArray(*holder,*(json.holder));//加入进去
	}
	else{
		//是引用 复制加入
		cJSON_AddItemToArray(*holder,cJSON_Duplicate(*json.holder,true));
	}
}
void Json::add_item(Json &json){
	Json::add_item(std::move(json));
}
void Json::add_item(const char *key,Json &&json){
	check_is_object();
	if(not json.holder->ref){
		//不是引用
		json.holder->ref = true;//改为引用
		cJSON_AddItemToObject(*holder,key,*json.holder);
	}
	else{
		//复制一下
		cJSON_AddItemToObject(*holder,key,cJSON_Duplicate(*json.holder,true));
	}
}
void Json::add_item(const char *key,Json &json){
	Json::add_item(key,std::move(json));
}
//弹出
Json Json::pop(int val){
	//在数组里面
	check_is_array();
	cJSON *cjson = cJSON_DetachItemFromArray((*holder),val);
	//分离出来的
	if(cjson == nullptr){
		throw IndexError(val);
	}
	return Json(cjson);
}
Json Json::pop(const char *key,bool case_sensitive){
	//弹出在字典里面
	check_is_object();
	cJSON *cjson;
	if(case_sensitive == true){
		//区分大小写
		cjson = cJSON_DetachItemFromObjectCaseSensitive((*holder),key);
	}
	else{
		cjson = cJSON_DetachItemFromObject((*holder),key);
	}
	if(cjson == nullptr){
		throw KeyError(key);
	}
	return Json(cjson);
}
//特殊操作
//输出到流
std::ostream& operator <<(std::ostream &stream,const Json &json){
	char *ch = json.to_cstring();
	stream << ch;
	Json::Free(ch);
	return stream;
}
Json *Json::clone() const{
	//克隆自己
	return new Json(cJSON_Duplicate((*holder),true));
}
JsonIterator Json::begin(){
	//得到开始的迭代器
	if(is_array() or is_object()){
		if(holder->get()->child == nullptr){
			//如果没有孩子
			throw NullPtrException();
		}
		return JsonIterator(holder->get()->child,nullptr);
	}
	throw TypeError("Array or Object",get_type_string());
}
JsonIterator Json::end(){
	//得到末尾的迭代器
	if(is_array() or is_object()){
		if(holder->get()->child == nullptr){
			//如果没有孩子
			throw NullPtrException();
		}
		cJSON *begin = holder->get()->child;//开始的节点
		while(begin->next != nullptr){
			//遍历到最后一个元素
			begin = begin->next;
		}
		return JsonIterator(nullptr,begin);
	}
	throw TypeError("Array or Object",get_type_string());
}
	//迭代器
	JsonIterator::JsonIterator(cJSON *cjson,cJSON *prev)
		:json(cjson,true){
		this->prev = prev;
	}
	JsonIterator::~JsonIterator(){

	}
	void JsonIterator::operator++(){
		//前往下一个
		cJSON *now = *json.holder;
		if(now == nullptr){
			throw NullPtrException();
		}
		if(now->next == nullptr){
			//如果下一个为nullptr
			prev = now;//保存当前指针 
		}
		//转到下一个
		json.holder->cjson = now->next;
	}
	void JsonIterator::operator--(){
		//前往前一个
		cJSON *now = *json.holder;
		if(now == nullptr){
			//如果当前指针为nullptr 代表为json.end()
			if(prev != nullptr){
				//设置为前一个
				json.holder->cjson = prev;
				prev = nullptr;//清空prev
				return;
			}
			throw NullPtrException();
		}
		if(now ->prev == nullptr){
			//如果前一个为nullptr
			throw NullPtrException();
		}
		json.holder->cjson = now -> prev;
	}
	bool JsonIterator::operator ==(const JsonIterator &iter){
		//判断Json是否相等
		return json.holder->cjson == iter.json.holder->cjson;
	}
	bool JsonIterator::operator !=(const JsonIterator &iter){
		//判断Json是否不相等
		return json.holder->cjson != iter.json.holder->cjson;
	}
};
