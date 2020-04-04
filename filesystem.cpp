#ifdef _WIN32
	#include <io.h>
	#include <direct.h>
	#define access _access
#else
	//Linux
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif
#include <cstring>
#include <cerrno>
#include "filesystem.hpp"
using namespace Box;
bool FS::Exists(const char *pathname){
	//判断文件是否存在
	if(access(pathname,F_OK) == 0){
		return true;
	}
	else{
		return false;
	}
}
bool FS::MkDir(const char *name){
	//创建文件夹
	#ifdef _WIN32
		//WIN32的创建
		if(_mkdir(name) == 0){
			return true;
		}
		else{
			return false;
		}
	#else
		if(mkdir(name,S_IRWXU) == 0){
			return true;
		}
		else{
			return false;
		}
	#endif
}
bool FS::Chdir(const char *path){
	//改变工作目录
	if(chdir(path) == 0){
		return true;
	}
	else{
		return false;
	}
}
const char *FS::GetError(){
	return strerror(errno);
}
