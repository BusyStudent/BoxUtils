#ifndef _BOX_BACKTRACE_HPP_
#define _BOX_BACKTRACE_HPP_
#include <vector>
#include <string>
#include "string.hpp"
namespace Box{
	String::Vector BackTrace(int max = 100);
	void PrintBackTrace(int max = 100);//打印回溯信息
	//得到C++的符号名字
	std::string DemangleName(const std::string &);
};
#endif
