#include <exception>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include "backtrace.hpp"
#include "assert.hpp"
#ifdef _WIN32
    #include <windows.h>
#endif
namespace Box{
    //断言失败后
    void AssertFail(const char *file,int line,const char *func,const char *exp){
        fprintf(stderr,"\033[31mError\033[0m Assertion failed \033[34m%s\033[0m:%d at Function:%s Expression:'%s'\n",file,line,func,exp);
        Box::Backtrace::Show();
        std::terminate();
    }
};