#ifndef _BOX_OS_HPP_
#define _BOX_OS_HPP_
//一些操作系统的API
#ifdef _WIN32
	#include <windows.h>
	#include "windows.hpp"
#else
	#include <unistd.h>
#endif
namespace Box{
	
};

#endif
