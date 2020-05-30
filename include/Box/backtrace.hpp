#ifndef _BOX_BACKTRACE_HPP_
#define _BOX_BACKTRACE_HPP_
#include <vector>
#include <string>
#include <cstddef>
namespace Box{
	namespace Backtrace{
		struct StackFrame{
			//调用栈层
			std::string name;//名字 可能是???
			void *addr;//地址
		};
		//这个命名空间的函数现在只有在Linux GLIBC上有实现
		std::vector <StackFrame> Get(int max = 100);
		void Show(int max = 100) noexcept;//打印在屏幕上
		int GetCallStack(void **stackptr,int max) noexcept;//得到调用堆栈
		//解析C++名字
		std::string Demangle(const std::string &) noexcept;
		std::string Demangle(const char *) noexcept;
		bool CollectFunction(const char *str,std::string *func,std::string *file) noexcept;//得到函数在字符串内
	};
};
#endif
