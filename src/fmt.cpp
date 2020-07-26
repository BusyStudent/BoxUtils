#define _BOX_SOURCE
#include <string>
#include <cstring>
#include <cstdarg>
#include <algorithm>
#include "fmt.hpp"
namespace Box{
namespace FmtImpl{
    BOXAPI std::string VFormat(const char *fmt,va_list varg){
        //可变参数
        std::string ret;
        //指针
        size_t len = strlen("{}");
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
};
};