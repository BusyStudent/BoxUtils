#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <vector>
#include <stdexcept>
#include "base64.hpp"
#include "string.hpp"
namespace Box{
	StringHelper::vector StringHelper::split(const std::string_view &delim){
		//切割字符串
		std::vector<std::string> vec;
		size_t len = strlen(delim.data());
		const char *pos = str;//开始查找位置
		const char *delim_pos;
		while(true){
			delim_pos = strstr(pos,delim.data());
			//查找
			if(delim_pos == nullptr){
				//没找到了把剩下部分压入
				vec.push_back(std::string(pos));
				break;
			}
			vec.push_back(std::string(pos,delim_pos));
			pos = delim_pos + len;
		}
		return vec;
	}
	StringHelper StringHelper::format(const std::string_view &fmt,...){
		//得到字符串大小
		size_t strsize;
		va_list varg;
		va_start(varg,fmt);
		strsize = vsnprintf(nullptr,0,fmt.data(),varg);
		va_end(varg);
		//申请内存
		char *str = static_cast<char*>(malloc((strsize + 1) * sizeof(char)));

		va_start(varg,fmt);
		vsnprintf(str,strsize,fmt.data(),varg);
		va_end(varg);
		return StringHelper(str,true);
	}
	StringHelper StringHelper::format(const std::string_view &fmt,va_list varg){
		char *str;
		vasprintf(&str,fmt.data(),varg);
		return StringHelper(str,true);
	}
	StringHelper::~StringHelper(){
		if(owned){
			free(const_cast<char*>(str));
		}
	}
	
};
