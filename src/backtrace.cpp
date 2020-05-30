#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <utility>
#include "string.hpp"
#include "backtrace.hpp"
//GLIBC特有的
#ifdef __gnu_linux__
	//GLIBC解析地址
	#include <execinfo.h>
#elif !defined(_WIN32)
	//POSIX的解析地址
	#include <unwind.h>
#endif
#ifdef __GNUC__
#include <cxxabi.h>
#endif
namespace Box{
namespace Backtrace{
	std::string Demangle(const std::string &name) noexcept{
		//GCC的函数
		return Demangle(name.c_str());
	}
	std::string Demangle(const char *name) noexcept{
		#ifdef __GNUC__
		//得到C++函数或者其他符号的名字
		int ok;
		char *real = abi::__cxa_demangle(name,nullptr,nullptr,&ok);
		if(ok == 0){
			//成功
			std::string real_name(real);
			free(real);
			return std::move(real_name);
		}
		#else
			#warning DemangleName Unsupported
		#endif
		//失败
		return name;
	}
	//解析名字
	bool CollectFunction(const char *str,std::string *func,std::string *file) noexcept{
		//解析函数从backtrace得到结果字符串中
		const char *p_begin = strchr(str,'(');
		//得到括号开始
		if(p_begin == nullptr){
			return false;
		}
		//如果需要文件信息
		if(file != nullptr){
			file->clear();
			file->append(str,p_begin - 1 - str);
		}
		const char *p_end = strchr(p_begin,')');
		//得到括号结束
		if(p_end == nullptr){
			return false;
		}
		const char *p_plus = strchr(p_begin,'+');
		//查找加号
		if(p_plus == nullptr or p_plus > p_end){
			//没找到或者超过了末尾
			if(func != nullptr){
				func->clear();
				func->append(p_begin + 1,p_end - p_begin - 1);
			}
		}
		else{
			//找到等号
			if(func != nullptr){
				func->clear();
				func->append(p_begin + 1,p_plus - p_begin - 1);
			}
		}
		return true;
	}	
	//得到调用堆栈地址
	int GetCallStack(void **stack_ptr,int max) noexcept{
		return backtrace(stack_ptr,max);
	};
	void Show(int max) noexcept{
		//打印调用堆栈
		void *callstack[max];
		//得到地址
		int ret = GetCallStack(callstack,max);
		if(ret <= 0){
			//失败
			return;
		}
		char **symbols = backtrace_symbols(callstack,ret);
		//得到调用堆栈的符号名字
		if(symbols == nullptr){
			return;
		}
		std::string func;//函数名字
		for(int i = 0; i < ret ; i ++){
			if(CollectFunction(symbols[i],&func,nullptr)){
				//如果得到正确名称 解出名称
				func = Demangle(func.c_str());
			}else{
				func = "???";
			}
			fprintf(stderr,"  at %p: %s\n",callstack[i],func.c_str());
		}
		free(symbols);
	}
	//得到堆栈详情
	std::vector<StackFrame> Get(int max){
		std::vector<StackFrame> frames;
		void *callstack[max];
		//得到地址
		int ret = GetCallStack(callstack,max);
		if(ret <= 0){
			//失败
			return frames;
		}
		char **symbols = backtrace_symbols(callstack,ret);
		if(symbols == nullptr){
			return frames;//返回空的
		}
		std::string func;//函数名字
		for(int i = 0; i< ret; i ++){
			if(CollectFunction(symbols[i],&func,nullptr)){
				//如果得到正确名称 解出名称
				func = Demangle(func.c_str());
			}else{
				func = "???";
			}
			//压入
			frames.push_back(
				StackFrame{
					.name = func,
					.addr = callstack[i]
				}
			);
		}

		return std::move(frames);
	}
}
}
