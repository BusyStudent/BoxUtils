#ifndef _BOX_LIBC_H_
#define _BOX_LIBC_H_
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
#endif