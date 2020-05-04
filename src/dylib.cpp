#include "dylib.hpp"
#ifdef _WIN32
	#include <window.hpp>
#else
	#include <dlfcn.h>
#endif
using namespace Box;
Dylib::Dylib(){
	//把指针给为nullptr
	handle = nullptr;
	err = "";
}
Dylib::~Dylib(){
	this->close();
}
bool Dylib::open(const char *lib,int mode){
	//打开库
	if(mode == 0){
		mode = RTLD_NOW;
	}
	handle = dlopen(lib,mode);
	if(handle == nullptr){
		//得到错误
		err = dlerror();
		return false;
	}
	return true;
}
bool Dylib::close(){
	if(handle == nullptr){
		return false;
	}
	if(dlclose(handle) == 0){
		//OK
		handle = nullptr;
		return true;
	}
	//得到错误
	err = dlerror();
	return false;
}
//得到错误
const char *Dylib::get_error() const{
	return err;
}
//查找符号
void *Dylib::find(const char *symbol){
	void *s = dlsym(handle,symbol);
	if(s == nullptr){
		//失败
		err = dlerror();
	}
	return s;
}
