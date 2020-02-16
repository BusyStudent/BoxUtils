#ifndef _BOXUTILS_JSON_HPP_
#define _BOXUTILS_JSON_HPP_
#include <string>
#include "cJSON.h"
namespace BoxUtils{
	class JsonIterator;
	class JsonArrayIterator;
	class JsonTableIterator;
	class Json{
		public:
			//初始化是是否独立的
			Json(cJSON *item,bool independence=true);
			Json(const Json &);
			~Json();
			//打印等调试
			void print();
			//复制
			Json *copy();
			const char *get_type_string();//得到类型字符串
			char *to_cstring();//转化字符串
			std::string to_string();
			//类型判断
			bool is_null();
			bool is_bool();
			bool is_true();//是真
			bool is_false();//是假
			bool is_string();//是否是字符串
			bool is_number();
			
			bool is_list();//是否是列表
			bool is_array();
			
			bool is_object();//是否是对象 或者说表
			bool is_table();
			
			//类型检查
			void check_is_array();//检查是数组
			void check_is_object();//检查是对象
			
			void add_string(const char *str);
			void add_string(const char *key,const char *str);
			void add_number(double number);
			
			void add_item(Json &item);//加入另一个节点到数组
			void add_item(const char *key,Json &item);//加入对象里面 不就是字典么
			//得到一些信息
			int get_array_size();
			JsonArrayIterator iter_array();//迭代数组
			JsonTableIterator iter_table();//迭代表
			//
			Json operator [](const char*);//查找数据
			Json operator [](int val);
			//比较Json
			bool operator == (const Json &json);
			void operator >> (int &i);//得到int
			void operator >> (double &number);//得到number
			void operator >> (char *&str);//得到复制过后的字符串
			void operator >> (std::string &str);
			void operator >> (const char * & str);//得到字符串
			void operator >> (bool &boolen);
			void operator << (int &i);
			void operator << (double &number);
			cJSON *get_cjson();
			//构造Json的类型
			static Json *ParseString(const char *);
			static Json *CreateNull();
			static Json *CreateBool(bool boolen);
			static Json *CreateTrue();
			static Json *CreateFalse();
			static Json *CreateTable();//创建一个表
			static Json *CreateArray();//创建一个数组
			static Json *CreateIntArray(int *array,int count);//创建一个Int的Array
			static Json *CreateNumber(double number);
			static Json *CreateString(const char *str);//创建一个字符串
			static Json *CreateStringRef(const char *str);//引用字符串
			//格式化字符串
			static void MinifyString(char *str);
			//文件函数
			static void  SaveFile(Json &json,const char *filename);
			static Json *LoadFile(const char *filename);
		private:
			bool independence;//是否独立
			cJSON *item;//对象
		friend class JsonArrayIterator;
		friend class JsonTableIterator;
	};
	class JsonIterator{
		public:
			JsonIterator();
			JsonIterator(const JsonIterator &iter);
			~JsonIterator();
			Json &operator *(){
				return *now_item;
			}
			Json *operator ->(){
				return now_item;
			}
		protected:
			int *refcount;
			Json *now_item = nullptr;
		friend class Json;
	};
	class JsonArrayIterator:public JsonIterator{
		public:
			bool operator ++(){
				cJSON *cjson = now_item->item->next;//得到下一个
				if(cjson == nullptr){
					return false;
				}
				now_item->item = cjson;//赋值一下
				return true;
			}
		friend class Json;
	};
	class JsonTableIterator:public JsonIterator{
		public:
			const char *name;
			bool operator ++(){
				cJSON *cjson = now_item->item->next;//得到下一个
				if(cjson == nullptr){
					return false;
				}
				now_item->item = cjson;//赋值一下
				name = cjson->string;
				return true;
			}
		friend class Json;
	};
};
#endif
