#ifndef _BOXUTILS_JSON_HPP_
#define _BOXUTILS_JSON_HPP_
#include "cJSON.h"
namespace BoxUtils{
	class Json{
		public:
			Json(cJSON *item);
			~Json();
			const char *to_string();//转化字符串
			//构造Json的类型
			static Json *ParseString(const char *);
			static Json *CreateNull();
			static Json *CreateBool(bool boolen);
			static Json *CreateTrue();
			static Json *CreateFalse();
			static Json *CreateNumber(double number);
			static Json *CreateString(const char *str);//创建一个字符串
			static Json *CreateStringRef(const char *str);//引用字符串
			//文件函数
			static Json *LoadFile(const char *filename);
		private:
			cJSON *item;//对象
	};
};
#endif
