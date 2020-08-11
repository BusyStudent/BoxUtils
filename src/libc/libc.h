#ifndef _BOX_LIBC_H_
#define _BOX_LIBC_H_
#define _BOX_SOURCE //确定是原代码
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "libc/attr.h"
#ifdef _WIN32
    #define LIBC_EXPORT __declspec(dllimport) __cdecl
#else
    #define LIBC_EXPORT 
#endif
#ifndef __cplusplus
    #define LIBC_BEGIN
    #define LIBC_END
    #define and &&
    #define not !
    #define or  ||

    #ifndef true
        #define true 1
    #endif
    #ifndef false
        #define false 0
    #endif
    #ifndef nullptr
        #define nullptr NULL
    #endif
#else
    #define LIBC_BEGIN extern "C"{
    #define LIBC_END   }
#endif
//检查 失败打印Error
#define LIBC_ASSERT_PERROR(VAL) \
    if(not (VAL)){\
        puts(strerror(errno));\
        abort();\
    }

#endif