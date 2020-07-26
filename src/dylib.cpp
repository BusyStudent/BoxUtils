#ifdef _WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif
#include <cerrno>
#include <stdexcept>
#include "dylib.hpp"
#include "exception.hpp"
namespace Box{
	//Windows的module
	#ifdef _WIN32
	struct WinModule{
		HMODULE mod;
	};
	#endif
	Library::Library(const char *filename){
		handle = OpenLibrary(filename);
		if(handle == nullptr){
			#ifdef _WIN32
			char buf[128];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
				nullptr,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				buf,
				sizeof(buf),
				nullptr);
			//格式化错误
			throw std::runtime_error(buf);
			#else
			throw std::runtime_error(dlerror());
			#endif
		}
	}
	Library::~Library(){
		CloseLibrary(handle);
	}
	void *Library::find(const char *name){
		return FindFunction(handle,name);
	}
	//一些函数
	void *OpenLibrary(const char *filename){
		#ifdef _WIN32
		HMODULE mod = LoadLibraryA(filename);
		//打开库
		if(mod == nullptr){
			return nullptr;
		}
		else{
			return new WinModule{
				mod
			};
		}
		#else
		return dlopen(filename,RTLD_LAZY);
		#endif
	};
	void *FindFunction(void *handle,const char *name){
		#ifdef _WIN32
		if(handle == nullptr){
			throwNullPtrException();
		}
		else{
			return reinterpret_cast<void*>(GetProcAddress(static_cast<WinModule*>(handle)->mod,name));
		}
		#else
		return dlsym(handle,name);
		#endif
	}
	bool  CloseLibrary(void *handle){
		#ifdef _WIN32
		WinModule *mod = static_cast<WinModule*>(handle);
		if(mod != nullptr){
			WINBOOL ret = FreeLibrary(mod->mod);
			delete mod;
			return ret == TRUE ? true : false;
		}
		return false;
		#else

		if(handle != nullptr){
			return dlclose(handle) == 0;
		}
		return false;
		#endif
	};
};