#include <exception>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include "common/def.hpp"
#include "backtrace.hpp"
#include "assert.hpp"
#ifdef _WIN32
    #include <windows.h>
    #include "fmt.hpp"
#endif
namespace Box{
    //断言失败后
    void AssertFail(const char *file,int line,const char *func,const char *exp){
        #ifndef _WIN32
        //类Unix
        fprintf(stderr,"\033[31mError\033[0m Assertion failed \033[34m%s\033[0m:%d at Function:%s Expression:'%s'\n",file,line,func,exp);
        #else
        //Window上错误提示框
        std::string msg = Fmt::Format("Error Assertion failed\n{}:{} at Function:{} Expression:'{}'",file,line,func,exp);
        fprintf(stderr,msg.c_str());
        fputc('\n',stderr);
        if(MessageBoxA(nullptr,msg.c_str(),"Error",MB_ICONERROR | MB_ABORTRETRYIGNORE) == IDIGNORE){
            //忽略
            return;
        }
        #endif
        Box::Backtrace::Show();
        std::terminate();
    }
};
