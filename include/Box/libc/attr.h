#ifndef _BOX_LIBC_EXT_HPP_
#define _BOX_LIBC_EXT_HPP_
//对编译器上 给的一些方法的扩展
#if defined(__GNUC__)
//GCC
    //设置别名
    #define BOX_MAKE_ALIAS(RET_TYPE,NEW_NAME,OLD_NAME) \
        RET_TYPE NEW_NAME() __attribute__ ((weak, alias (#OLD_NAME)))
#elif defined(_MSC_VER)
//MSVC

#endif
#endif