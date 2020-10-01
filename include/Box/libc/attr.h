#ifndef _BOX_LIBC_EXT_HPP_
#define _BOX_LIBC_EXT_HPP_
//对编译器上 给的一些方法的扩展
#if defined(__GNUC__) || defined(__TINYC__)
//GCC or Tcc
    //设置别名
    #define BOX_MAKE_ALIAS(RET_TYPE,NEW_NAME,OLD_NAME) \
        RET_TYPE NEW_NAME() __attribute__ ((weak, alias (#OLD_NAME)))
    //构造器 在main之前调用
    #define BOX_CONSTRUCTOR __attribute__((constructor))
    //在main之后调用
    #define BOX_DESTRUCTOR  __attribute__((destructor))
    //CALL
    #define BOX_STDCALL     __attribute__((stdcall))
    #define BOX_CDECL       __attribute__((cdecl))
    //inline
    #define BOX_NOINLINE    __attribute__((noinline))
    #define BOX_INLINE      __attribute__((always_inline))
    //deprecated
    #define BOX_DEPRECATED  __attribute__((deprecated))
    //noreturn
    #define BOX_NORETURN    __attribute__((noreturn))
    //dllimport
    #ifdef _WIN32
        #define BOX_IMPORT      __attribute__((dllimport))
        #define BOX_EXPORT      __attribute__((dllexport))
    #else
        #define BOX_IMPORT
        #define BOX_EXPORT
    #endif
#elif defined(_MSC_VER) 
//MSVC
    #define BOX_EXPORT __declspec(dllexport)
    #define BOX_IMPORT __declspec(dllimport)
    #define BOX_INLINE __forceinline
    #define BOX_NORETURN __declspec(noreturn)
#else
    #warning do not support it
#endif

//Thread local storage
#ifdef __cplusplus
    #define BOX_TLS thread_local
#elif defined(__GNUC__)
    #define BOX_TLS __thread
#elif defined(_MSC_VER)
    #define BOX_TLS __declspec(thread)
#endif

#ifdef _BOX_SOURCE
    //是源代码文件
    #define BOXAPI BOX_EXPORT
#else
    //不是 就忽略了
    #define BOXAPI BOX_IMPORT
#endif

#endif