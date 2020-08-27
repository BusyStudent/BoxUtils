#ifndef _BOX_FMT_HPP_
#define _BOX_FMT_HPP_
//简单的Fmt 在C++20之前用用后面直接用std::format或者fmt::format
#include <iosfwd>
#include <string>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <exception>
#include <string_view>
#include "libc/attr.h"
namespace Box{
    namespace FmtImpl{
        //Fmt的实现
        extern BOXAPI std::string Format(const char *fmt,...);
        extern BOXAPI std::string VFormat(const char *fmt,va_list varg);
        //输出到流里面
        extern BOXAPI std::ostream &FPrintf(std::ostream&,const char *fmt,...);
        extern BOXAPI std::ostream &VFPrintf(std::ostream&,const char *fmt,va_list varg);
        //输出到C的流
        extern BOXAPI void FPrintf(FILE*,const char *fmt,...);
        extern BOXAPI void VFPrintf(FILE*,const char *fmt,va_list varg);
        //输出到stdout 
        extern BOXAPI void Printf(const char *fmt,...);
        extern BOXAPI void VPrintf(const char *fmt,va_list varg);
    } // namespace FmtImpl
    namespace Fmt{
        //到字符串
        template<class T>
        std::string ToString(const T &val){
            return std::to_string(val);
        };
        //格式化指针
        template<class T>
        std::string ToString(T *ptr){
            char buf[20] = {'\0'};//这个指针字符串长度
            sprintf(buf,"%p",ptr);
            return buf;
        };
        //字符串的包装器
        template<>
        inline std::string ToString<std::string>(const std::string &str){
            return str;
        };
        template<>
        inline std::string ToString<std::string_view>(const std::string_view &str){
            return std::string(str);
        };
        //异常格式化
        template<>
        inline std::string ToString<std::exception>(const std::exception &exp){
            return exp.what();
        };
        template<>
        inline std::string ToString<bool>(const bool &val){
            if(val){
                return "true";
            }
            else{
                return "false";
            }
        };
        inline std::string ToString(const char *str){
            return str;
        };
        inline std::string ToString(char *str){
            return str;
        };
        //格式化
        template<class ...Args>
        std::string Format(const char *fmt,const Args &...args){
            return FmtImpl::Format(fmt,ToString(args).c_str()...);
        };
        //C++ String
        template<class ...Args>
        std::string Format(const std::string &fmt,const Args &...args){
            return Fmt::Format(fmt.c_str(),args...);
        };
        //Print
        template<class ...Args>
        void Printf(FILE *fptr,const char *fmt,const Args &...args){
            return FmtImpl::FPrintf(fptr,fmt,ToString(args).c_str()...);
        };
        template<class ...Args>
        void Printf(const char *fmt,const Args &...args){
            return FmtImpl::Printf(fmt,ToString(args).c_str()...);
        };
        //Printfln
        template<class ...Args>
        void Printfln(FILE *fptr,const char *fmt,const Args &...args){
            FmtImpl::FPrintf(fptr,fmt,ToString(args).c_str()...);
            fputc('\n',fptr);
        };
        template<class ...Args>
        void Printfln(const char *fmt,const Args &...args){
            FmtImpl::Printf(fmt,ToString(args).c_str()...);
            putchar('\n');
            fflush(stdout);
        };
    } // namespace Fmt
    //在C++20之前勉强用用的format
    template<class T,class ...Args>
    std::string Format(const T &fmt,const Args &...args){
        return Fmt::Format(fmt,args...);
    };
    using Fmt::Printf;
    using Fmt::Printfln;
} // namespace Box

#endif