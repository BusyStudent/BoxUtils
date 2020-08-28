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
        struct StaticString{
            //C的静态字符串
            const char *data;
            const char *c_str() const noexcept{
                return data;
            }
        };
        //静态字符串版本
        inline StaticString ToString(const char *str){
            return StaticString{
                .data = str
            };
        }
        inline StaticString ToString(char *str){
            return StaticString{
                .data = str
            };
        }
        inline StaticString ToString(std::string_view str){
            return StaticString {
                .data = str.data()
            };
        }
        //字符串的包装器
        inline StaticString ToString(const std::string &str){
            return {
                .data = str.c_str()
            };
        };
        //异常格式化
        inline StaticString ToString(const std::exception &exp){
            return {
                .data = exp.what()
            };
        };
        inline StaticString ToString(bool val){
            if(val){
                return {
                    .data = "true"
                };
            }
            else{
                return {
                    .data = "false"
                };
            }
        };
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
        //直接想打印
        template<class T>
        void Printfln(FILE *fptr,const T &data){
            fputs(ToString(data).c_str(),fptr);
            fputc('\n',fptr);
        };
        template<class T>
        void Printfln(const T &data){
            puts(ToString(data).c_str());
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