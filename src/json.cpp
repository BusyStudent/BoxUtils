#define _BOX_SOURCE
#include <functional>
#include <cstring>
#include <string>
#include <fstream>
#include <ostream>
#include <utility>
#include <mutex>
#include <exception>
#include "common/def.hpp"
#include "raii/scope.hpp"
#include "libc/mem.h"
#include "exception.hpp"
#include "cJSON_Utils.h"
#include "cJSON.h"
#include "json.hpp"
#include "fmt.hpp"
namespace Box{
	//JsonRef::Helper实现
	using HelperImpl =  JsonRef::Helper;
	HelperImpl::Helper(int val){
		value = cJSON_CreateNumber(val);
	}
	HelperImpl::Helper(bool val){
		value = cJSON_CreateBool(val);
	}
	HelperImpl::Helper(float val){
		value = cJSON_CreateNumber(val);
	}
	HelperImpl::Helper(double val){
		value = cJSON_CreateNumber(val);
	}
	HelperImpl::Helper(const char *str){
		value = cJSON_CreateString(str);
	}
	HelperImpl::Helper(const std::string &str){
		value = cJSON_CreateString(str.c_str());
	}
	HelperImpl::Helper(std::string_view str){
		value = cJSON_CreateString(str.data());
	}
	HelperImpl::Helper(std::initializer_list<Helper> vlist){
		value = cJSON_CreateArray();
		for(auto &each:vlist){
			cJSON_AddItemToArray(value,each.value);
		}
	}
	
};
namespace Box{
	//JsonRef的实现空间
	//JsonRef转换数值
	template<>
	int JsonRef::get_value<int>() const{
		check_is_number();
		return value->valueint;
	};
	template<>
	bool JsonRef::get_value<bool>() const{
		check_is_bool();
		return value->valueint;
	};
	template<>
	float JsonRef::get_value<float>() const{
		check_is_number();
		return value->valuedouble;
	};
	template<>
	double JsonRef::get_value<double>() const{
		check_is_number();
		return value->valuedouble;
	};
	template<>
	std::string JsonRef::get_value<std::string>() const{
		check_is_string();
		return value->valuestring;
	};
	template<>
	std::string_view JsonRef::get_value<std::string_view>() const{
		check_is_string();
		return value->valuestring;
	};
	//设置数值
	void JsonRef::set_value(int val){
		check_is_number();
		cJSON_SetIntValue(value,val);
	}
	void JsonRef::set_value(bool val){
		check_is_bool();
		cJSON_SetIntValue(value,val);
	}
	void JsonRef::set_value(float val){
		check_is_number();
		cJSON_SetNumberValue(value,val);
	}
	void JsonRef::set_value(double val){
		check_is_number();
		cJSON_SetNumberValue(value,val);
	}
	void JsonRef::set_value(JsonRef ref){
		if((ref.is_array() or ref.is_object())){
			if(is_array() or is_object()){
				clear();
				//一个个加入
				cJSON *elem;
				cJSON_ArrayForEach(elem,ref.value){
					cJSON_AddItemToArray(value,
						cJSON_Duplicate(elem,1)
					);
				}
				return;
			}
		}
		throwTypeError("Array or Object",Format(
			"Self:{} Arg:{}",type(),ref.type()
		));
	
	}
	void JsonRef::set_value(const Helper &helper){
		if(cJSON_IsArray(helper.value) or cJSON_IsObject(helper.value)){
			if(is_array() or is_object()){
				clear();
				//一个个加入
				cJSON *elem = cJSON_DetachItemFromArray(helper.value,0);
				while(elem != nullptr){
					cJSON_AddItemToArray(value,elem);
					elem = cJSON_DetachItemFromArray(helper.value,0);
				}
				//释放原来的东西
				cJSON_Delete(helper.value);
				return;
			}
		}
		throwTypeError("Array or Object",Format(
			"Self:{} Arg:{}",type(),JsonRef(helper.value).type()
		));
	}
	void JsonRef::set_value(std::string_view str){
		check_is_string();
		cJSON_free(value->valuestring);
		//拷贝字符串
		value->valuestring = libc::strdup(str.data(),cJSON_malloc);
	}
	//Json&&的set value
	void JsonRef::set_value(Json &&json){
		//检查类型
		if((json.is_array() or json.is_object())){
			if(is_array() or is_object()){
				//清空原来地
				clear();
				//一个个分离 加入
				cJSON *elem = cJSON_DetachItemFromArray(json.value,0);
				while(elem != nullptr){
					cJSON_AddItemToArray(value,elem);
					elem = cJSON_DetachItemFromArray(json.value,0);
				}
				return;
			}
		}
		throwTypeError("Array or Object",Format(
			"Self:{} Arg:{}",type(),json.type()
		));
	}
	//得到类型
	std::string_view JsonRef::type() const noexcept{
		switch(value->type & 0xFF){
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
	//得到path
	std::string JsonRef::get_path(JsonRef ref) const{
		char *path = cJSONUtils_FindPointerFromObjectTo(value,ref.value);
		if(path == nullptr){
			throwNotFoundError(
				Format("Cannot get by from:{} to:{}",value,ref.value)
			);
		}
		std::string p(path);
		cJSON_free(path);
		return p;
	}
	int JsonRef::size() const{
		//得到数组大小
		if(is_object() or is_array()){
			//检查是数组或者是字典
			return cJSON_GetArraySize(value);
		}
		throwTypeError("Array or Object",type());
	}
	std::string_view JsonRef::name() const{
		if(value->string == nullptr){
			throwNullPtrException();
		}
		return value->string;
	}
	//类型判断
	//类型判断
	bool JsonRef::is_bool() const{
		return (bool)cJSON_IsBool(value);
	}
	bool JsonRef::is_true() const{
		return (bool)cJSON_IsTrue(value);
	}
	bool JsonRef::is_false() const{
		return (bool)cJSON_IsFalse(value);
	}
	bool JsonRef::is_list() const{
		//是否是列表 和数组一样
		return (bool)cJSON_IsArray(value);
	}
	bool JsonRef::is_null() const{
		return (bool)cJSON_IsNull(value);
	}
	bool JsonRef::is_array() const{
		return (bool)cJSON_IsArray(value);
	}
	bool JsonRef::is_number() const{
		return (bool)cJSON_IsNumber(value);
	}
	bool JsonRef::is_string() const{
		return (bool)cJSON_IsString(value);
	}
	//
	bool JsonRef::is_object() const{
		return (bool)cJSON_IsObject(value);
	}
	bool JsonRef::is_table() const{
		return (bool)cJSON_IsObject(value);
	}
	//类型检查
	//类型检查
	//这里复制了一点cJSON的源代码
	void JsonRef::check_is_array() const{
		if((value->type & 0xFF) != cJSON_Array){
			throwTypeError("Array",type());
		}
	}
	void JsonRef::check_is_object() const{
		if((value->type & 0xFF) != cJSON_Object){
			throwTypeError("Object",type());
		}
	}
	void JsonRef::check_is_string() const{
		if((value->type & 0xFF) != cJSON_String){
			throwTypeError("String",type());
		}
	}
	void JsonRef::check_is_number() const{
		if((value->type & 0xFF) != cJSON_Number){
			throwTypeError("Number",type());
		}
	}
	void JsonRef::check_is_bool() const{
		if((value->type & (cJSON_True | cJSON_False)) != 0){
			throwTypeError("Bool",type());
		}
	}
	//插入到数组
	void JsonRef::insert(JsonRef ref){
		check_is_array();
		cJSON_AddItemToArray(value,
			cJSON_Duplicate(ref.value,1)
		);
	}
	void JsonRef::insert(Json &&json){
		check_is_array();
		cJSON *val = json.value;
		//减少引用
		json.value = nullptr;//移走内容
		json.unref();
		cJSON_AddItemToArray(value,val);	
	}
	void JsonRef::insert(const Helper &helper){
		check_is_array();
		cJSON_AddItemToArray(value,helper.value);
	}
	//插入到数组或者表
	void JsonRef::insert(int index,JsonRef ref){
		//这是引用 会拷贝一下原来副本
		if(not(is_array() or is_object())){
			throwTypeError("Array or Object",type());
		}
		cJSON_InsertItemInArray(value,index,
			cJSON_Duplicate(ref.value,1)
		);
	}
	void JsonRef::insert(int index,Json &&json){
		if(not(is_array() or is_object())){
			throwTypeError("Array or Object",type());
		}
		cJSON *val = json.value;
		//减少引用
		json.value = nullptr;//移走内容
		json.unref();
		cJSON_InsertItemInArray(value,index,val);
	}
	void JsonRef::insert(int index,const Helper &helper){
		if(not(is_array() or is_object())){
			throwTypeError("Array or Object",type());
		}
		cJSON_InsertItemInArray(value,index,helper.value);
	}
	//添加到表里面
	void JsonRef::insert(std::string_view str,JsonRef ref){
		check_is_object();
		cJSON_AddItemToObject(value,str.data(),
			cJSON_Duplicate(ref.value,1)
		);
	}
	void JsonRef::insert(std::string_view str,Json &&json){
		check_is_object();
		cJSON *val = json.value;
		//减少引用
		json.value = nullptr;//移走内容
		json.unref();
		cJSON_AddItemToObject(value,str.data(),val);
	}
	void JsonRef::insert(std::string_view str,const Helper &helper){
		check_is_object();
		cJSON_AddItemToObject(value,str.data(),helper.value);
	}
	//移除所有项
	void JsonRef::clear(){
		if(not(is_array() or is_object())){
			throwTypeError("Array or Object",type());
		}
		//根据实现这样是可以的
		cJSON *val = cJSON_DetachItemFromArray(value,0);
		while(val != nullptr){
			cJSON_Delete(val);
			val = cJSON_DetachItemFromArray(value,0);
		}
	}
	//弹出
	Json JsonRef::pop(int index){
		check_is_array();
		cJSON *elem = cJSON_DetachItemFromArray(value,index);
		if(elem == nullptr){
			throwIndexError(index);
		}
		else{
			return Json(elem);
		}
	}
	//是否有
	bool JsonRef::has(int index) const{
		check_is_array();
		return cJSON_GetArrayItem(value,index) != nullptr;
	}
	bool JsonRef::has(std::string_view key,bool case_sensitive) const{
		check_is_array();
		if(case_sensitive){
			return cJSON_GetObjectItemCaseSensitive(value,key.data());
		}
		else{
			return cJSON_GetObjectItem(value,key.data());
		}
	}
	//operator
	//查找
	JsonRef JsonRef::operator [](int index) const{
		check_is_array();
		cJSON *val = cJSON_GetArrayItem(value,index);
		if(val == nullptr){
			throwIndexError(index);
		}
		else{
			return val;
		}
	}
	JsonRef JsonRef::operator [](std::string_view str) const{
		check_is_object();
		cJSON *val = cJSON_GetObjectItemCaseSensitive(value,str.data());
		if(val == nullptr){
			throwKeyError(str.data());
		}
		else{
			return val;
		}
	}
	//比较
	bool JsonRef::operator ==(int v) const noexcept{
		return is_number() and value->valueint == v;
	}
	bool JsonRef::operator ==(bool v) const noexcept{
		return is_bool() and value->valueint == v;
	}
	bool JsonRef::operator ==(float v) const noexcept{
		return is_number() and static_cast<float>(value->valuedouble) == v;
	}
	bool JsonRef::operator ==(double v) const noexcept{
		return is_number() and value->valuedouble == v;
	}
	bool JsonRef::operator ==(JsonRef ref) const noexcept{
		return cJSON_Compare(value,ref.value,true);
	}
	bool JsonRef::operator ==(std::nullptr_t ptr) const noexcept{
		return is_null() and ptr == nullptr;
	}
	bool JsonRef::operator ==(std::string_view str) const noexcept{
		return is_string() and value->valuestring == str;
	}
	bool JsonRef::operator ==(const Json &json) const noexcept{
		return cJSON_Compare(value,json.value,true);
	}
	//到字符串
	std::string JsonRef::to_string(bool formated) const{
		char *str = cJSON_PrintBuffered(value,0,formated);
		if(str == nullptr){
			throwNullPtrException();
		}
		Box_defer{
			cJSON_free(str);
		};
		return str;
	}
	//ostream转换
	std::ostream &operator <<(std::ostream &ostr,JsonRef ref){
		char *str = cJSON_Print(ref.value);
		Box_defer{
			cJSON_free(str);
		};
		return ostr << str;
	}
	//迭代器
	JsonIterator JsonRef::erase(JsonIterator iter){
		if(is_array() or is_object()){
			//删除分离内容
			cJSON *prev = iter->value->prev;
			//保存前一个
			cJSON *next = iter->value->next;
			cJSON_DetachItemViaPointer(value,iter->value);
			cJSON_Delete(iter->value);
			return JsonIterator(next,prev);
		}
		else{
			throwTypeError("Array or Object",type());
		}
	}
	JsonIterator JsonRef::begin(){
		if(is_array() or is_object()){
			cJSON *child = value->child;
			return JsonIterator(child,nullptr);
		}
		else{
			throwTypeError("Array or Object",type());
		}
	}
	JsonIterator JsonRef::end(){
		if(is_array() or is_object()){
			if(value->child == nullptr){
				//没有内容
				return JsonIterator(nullptr,nullptr);
			}
			cJSON *prev = value->child;
			//一直到末尾
			while(prev->next != nullptr){
				prev = prev->next;
			}
			return JsonIterator(nullptr,prev);
		}
		else{
			throwTypeError("Array or Object",type());
		}
	}
}
namespace Box{
	Json::Json(cJSON *val):JsonRef(val){
		refcount = new RefCount{
			1
		};
	}
	Json::Json(const Json &json):
		JsonRef(json.value),
		refcount(json.refcount){
		//复制一下
		ref();//增加引用
	}
	Json::Json(Json &&json):
		JsonRef(json.value),
		refcount(json.refcount){
		//复制一下
		//拿走计数器
		json.refcount = nullptr;
	}
	Json::Json(const Helper &helper):
		JsonRef(helper.value){
		refcount = new RefCount{
			1
		};
	}
	Json::~Json(){
		Json::unref();
	}
	Json Json::clone() const{
		if(value == nullptr){
			throwNullPtrException();
		}
		else{
			return cJSON_Duplicate(value,1);
		}
	}
	Json Json::ParseString(std::string_view str){
		#if !defined(__GNUC__) &&  !defined(_MSC_VER)
		//线程不安全 加锁
		static std::mutex mtx;
		std::lock_guard<std::mutex> locker(mtx);
		#endif
		cJSON *cjson = cJSON_Parse(str.data());
		if(cjson == nullptr){
			//解析失败
			throw JsonParseError(GetError());
		}
		return Json(cjson);
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
	Json Json::CreateString(std::string_view str){
		return Json(cJSON_CreateString(str.data()));
	}
	Json Json::CreateStringRef(std::string_view str){
		return Json(cJSON_CreateStringReference(str.data()));
	}
	Json Json::LoadFile(std::string_view filename){
		std::ifstream stream(filename.data());
		//打开文件
		std::istreambuf_iterator<char> beg(stream),end;
		std::string str(beg,end);
		if(stream.bad()){
			//失败
			throw FileNotFoundError(errno,filename.data());
		}
		//读入所有字符
		Box_defer{
			stream.close();
		};
		return ParseString(str);//解析字符串
	}
	Json Json::ParseFrom(std::istream &stream){
		//从流解析
		std::istreambuf_iterator<char> beg(stream),end;
		std::string str(beg,end);
		return ParseString(str);
	}
	void Json::SaveFile(Json &json,std::string_view filename){
		//写出文件
		std::ofstream stream(filename.data());
		stream << json.to_string();
	}
	//简化字符串
	void Json::MinifyString(char *str){
		cJSON_Minify(str);
	}
	void Json::MinifyString(std::string &str){
		cJSON_Minify(str.data());
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
	void *Json::malloc(size_t size){
		return cJSON_malloc(size);
	}
	void Json::free(void *ptr){
		cJSON_free(ptr);
	}
	//引用部分
	void Json::unref(){
		if(refcount != nullptr){
			refcount->count --;//减去计数
			if(refcount->count <= 0){
				cJSON_Delete(value);
				delete refcount;
				refcount = nullptr;
			}
		}
	}
	void Json::ref(){
		//增加引用
		(refcount->count) ++;
	}
};
//迭代器
namespace Box{
	std::string_view JsonIterator::key() const{
		if(json.value == nullptr){
			throwNullPtrException();
		}
		if(json.value->string == nullptr){
			throwNullPtrException();
		}
		return json.value->string;
	}
	void JsonIterator::operator ++(){
		//前进
		if(json.value != nullptr){
			prev = json.value;//保存原来的是前一个
			json.value = json.value->next;
		}
	}
	void JsonIterator::operator --(){
		if(json.value == nullptr){
			//前一个数值是
			if(prev != nullptr){
				json.value = prev;
				//前往前面一个
				prev = prev->prev;
			}
			else{
				//没办法
				throwNullPtrException();
			}
		}
		else{
			json.value = json.value->prev;
			if(json.value != nullptr){
				//不为空
				prev = json.value->prev;
			}
			else{
				prev =  nullptr;
			}
		}
	}
	JsonIterator JsonIterator::operator ++(int){
		JsonIterator iter = *this;
		++(*this);
		return iter;
	}
	JsonIterator JsonIterator::operator --(int){
		JsonIterator iter = *this;
		--(*this);
		return iter;
	}
};