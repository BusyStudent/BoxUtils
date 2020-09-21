#include <string>
#include <cstring>
#include <cstdarg>
#include <ostream>
#include <algorithm>
#include "common/def.hpp"
#include "raii/scope.hpp"
#include "fmt.hpp"
#define STATIC_STRLEN(STRING) (sizeof(STRING) - sizeof("")) / sizeof(char)
namespace Box{
namespace FmtImpl{
    BOXAPI std::string VFormat(const char *fmt,va_list varg){
        //可变参数
        std::string ret;
        //指针
        size_t len = STATIC_STRLEN("{}");
        const char *ptr = nullptr;
        ptr = strstr(fmt,"{}");//查找
        while(ptr != nullptr){
            ret.append(fmt,(ptr - fmt) / sizeof(char));
            //加入格式化之前的字符串
            ret += va_arg(varg,const char*);
            //加入格式化字符串
            fmt = ptr + len;//移动到后面
            ptr = strstr(fmt,"{}");//查找
        }
        ret += fmt;//加入剩下的字符串
        return ret;
    };
    BOXAPI std::string Format(const char *fmt,...){
        va_list varg;
        va_start(varg,fmt);
        std::string ret = VFormat(fmt,varg);
        va_end(varg);
        return ret;
    };
    //输出到流里面
    BOXAPI std::ostream &VFPrintf(std::ostream &ostr,const char *fmt,va_list varg){
        size_t len = STATIC_STRLEN("{}");
        const char *ptr = nullptr;
        ptr = strstr(fmt,"{}");//查找
        while(ptr != nullptr){
            ostr.write(fmt,(ptr - fmt) / sizeof(char));
            //写出格式化之前的字符串
            ostr << va_arg(varg,const char*);
            //写出格式化字符串
            fmt = ptr + len;//移动到后面
            ptr = strstr(fmt,"{}");//查找
        }
        ostr << fmt;
        return ostr;
    };
    BOXAPI std::ostream &FPrintf(std::ostream &ostr,const char *fmt,...){
        va_list varg;
        va_start(varg,fmt);

        Box_defer{
            va_end(varg);
        };
        return VFPrintf(ostr,fmt,varg);
    };
    //C版本的
    BOXAPI void VFPrintf(FILE *fptr,const char *fmt,va_list varg){
        size_t len = STATIC_STRLEN("{}");
        const char *ptr = nullptr;
        ptr = strstr(fmt,"{}");//查找
        while(ptr != nullptr){
            fwrite(fmt,sizeof(char),(ptr - fmt) / sizeof(char),fptr);
            //加入格式化之前的字符串
            fputs(va_arg(varg,const char*),fptr);
            //加入格式化字符串
            fmt = ptr + len;//移动到后面
            ptr = strstr(fmt,"{}");//查找
        }
        fputs(fmt,fptr);//打印剩下的
    }
    BOXAPI void FPrintf(FILE *fptr,const char *fmt,...){
        va_list varg;
        va_start(varg,fmt);

        Box_defer{
            va_end(varg);
        };
        return VFPrintf(fptr,fmt,varg);
    };
    BOXAPI void Printf(const char *fmt,...){
        va_list varg;
        va_start(varg,fmt);

        Box_defer{
            va_end(varg);
        };
        return VFPrintf(stdout,fmt,varg);
    }
    BOXAPI void VPrintf(const char *fmt,va_list varg){
        return VFPrintf(stdout,fmt,varg);
    }
};
};