#ifndef _BOX_FILESYSTEM_HPP_
#define _BOX_FILESYSTEM_HPP_
#include <cstdio>
#include <string>
#ifdef _WIN32
	#include <direct.h>
	#include <dirent.h>
	#include <sys/stat.h>
#else
	#include <dirent.h>
	#include <sys/stat.h>
#endif
#include "refptr.hpp"
//简单的文件系统操作
namespace Box{
	namespace FS{
		#ifdef _WIN32
		typedef struct _stat64 NativeStat64;
		#else
		typedef struct  stat64 NativeStat64;
		#endif
		//本地的stat
		struct Stat:public NativeStat64{
			bool is_reg() const;//是正常文件
			bool is_dir() const;//是目录
			bool is_fifo() const;//是管道
		};
		std::string Getcwd();
		size_t GetSize(const char *pathname);//得到文件大小
		
		void GetStat(const char *pathname,Stat &);//得到文件状态
		Stat GetStat(const char *pathname);//得到状态
		bool Exists(const char *pathname);//文件是否存在
		void MkDir(const char *name);//创建文件
		void Chdir(const char *path);//改变目录
		const char *GetError();//得到错误
	};
}
#endif
