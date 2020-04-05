#include <cstdlib>
#include <string>
#include <vector>
#include "string.hpp"
#include "backtrace.hpp"
//GLIBC特有的
#ifdef __gnu_linux__
#include <execinfo.h>
#endif
#ifdef __GNUC__
#include <cxxabi.h>
#endif
using namespace Box;

std::string Box::DemangleName(const std::string &s){
	//GCC的函数
	#ifdef __GNUC__
	//得到C++函数或者其他符号的名字
	int ok;
	char *real = abi::__cxa_demangle(s.c_str(),nullptr,nullptr,&ok);
	if(ok == 0){
		//成功
		std::string real_name = real;
		free(real);
		return real_name;
	}
	#else
		#warning DemangleName Unsupported
	#endif
	return s;
}
String::Vector Box::BackTrace(int max){
	#ifdef __gnu_linux__
	//得到堆栈追踪
	void *array[max];
	//定义数组
	int ret = backtrace(array,max);
	//得到有多少层
	char **str_array;
	str_array = backtrace_symbols(array,ret);
	//得到符号
	String::Vector vec;
	for(int i = 0;i < ret ;i++){
		std::string src = str_array[i];
		auto func_begin = src.find('(');//得到开始
		if(func_begin != std::string::npos){
			auto func_end = src.find('+');
			if(func_end != std::string::npos){
				//找到函数的主体部分
				auto s = src.substr(func_begin + 1,func_end - func_begin - 1);
				if(s == ""){
					//啥都没有
					vec.push_back("???");
				}
				else{
					//解析C++的符号
					vec.push_back(DemangleName(s));
				}
				continue;
			}
		}
		//没找到 直接压入???
		vec.push_back("???");
	}
	free(str_array);
	return vec;
	#else
	#warning function Box::BackTrace Unsupported 
	String::Vector vec;
	vec.push_back("Unsupport Platfrom");
	return vec;
	#endif
}
