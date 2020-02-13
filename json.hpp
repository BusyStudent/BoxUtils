#ifndef _BOXUTILS_JSON_HPP_
#define _BOXUTILS_JSON_HPP_
#include "cJSON.h"
namespace BoxUtils{
	class Json{
		public:
			//初始化是是否独立的
			Json(cJSON *item,bool independence=true);
			Json(const Json &);
			~Json();
			//复制
			Json *copy();
			const char *get_type_string();//得到类型字符串
			const char *to_string();//转化字符串
			//类型判断
			bool is_null();
			bool is_bool();
			bool is_string();//是否是字符串
			bool is_number();
			
			bool is_list();//是否是列表
			bool is_array();
			
			bool is_object();//是否是对象 或者说表
			bool is_table();
			//
			Json operator [](const char*);//查找数据
			Json operator [](int val);
			//比较Json
			bool operator == (const Json &json);
			void operator >> (int &i);//得到int
			void operator >> (double &number);//得到number
			void operator >> (char *&str);//得到复制过后的字符串
			void operator >> (const char * & str);//得到字符串
			void operator >> (bool &boolen);
			cJSON *get_cjson();
			//构造Json的类型
			static Json *ParseString(const char *);
			static Json *CreateNull();
			static Json *CreateBool(bool boolen);
			static Json *CreateTrue();
			static Json *CreateFalse();
			static Json *CreateNumber(double number);
			static Json *CreateString(const char *str);//创建一个字符串
			static Json *CreateStringRef(const char *str);//引用字符串
			//格式化字符串
			void MinifyString(char *str);
			//文件函数
			static Json *LoadFile(const char *filename);
		private:
			bool independence;//是否独立
			cJSON *item;//对象
	};
};
#endif
