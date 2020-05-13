#ifndef _BOX_OS_HPP_
#define _BOX_OS_HPP_
//一些操作系统的API
#ifdef _WIN32
	#include <windows.h>
	#include "windows.hpp"
#else
	#include <unistd.h>
	#include "linux.hpp"
#endif
namespace Box{
	#ifdef _WIN32
	namespace OS = Windows;
	#else
	namespace OS = Linux;
	#endif
};

#endif
