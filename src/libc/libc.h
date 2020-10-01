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
//Thread local
//README 大部分都没有threads.h
#if defined(__GUNC__)
    //GCC
    #define LIBC_TLS __thread
#elif defined(_MSC_VER)
    #define LIBC_TLS __declspec(thread)
#endif
//Mutex
#ifdef _WIN32
    #include <windows.h>
    typedef HANDLE libc_mutex_t;
    #define LIBC_MUTEX_LOCK(M) WaitForSingleObject((M),INFINITE)
    #define LIBC_MUTEX_UNLOCK(M) ReleaseMutex((M))
    #define LIBC_MUTEX_INIT(M) (M) = CreateMutex(nullptr,FALSE,nullptr)
    #define LIBC_MUTEX_DESTROY(M) CloseHandle((M))
#else
    #include <pthread.h>
    typedef pthread_mutex_t libc_mutex_t;
    #define LIBC_MUTEX_LOCK(M) pthread_mutex_lock(&(M))
    #define LIBC_MUTEX_UNLOCK(M) pthread_mutex_unlock(&(M))
    #define LIBC_MUTEX_INIT(M) pthread_mutex_init(&(M),nullptr)
    #define LIBC_MUTEX_DESTROY(M) pthread_mutex_destroy(&(M))
#endif
#define LIBC_ALLOC(TYPE) malloc(sizeof(TYPE))
#endif