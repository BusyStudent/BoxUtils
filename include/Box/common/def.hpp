#ifndef _BOX_COMMON_DEF_HPP_
#define _BOX_COMMON_DEF_HPP_
//原代码的 必须要包括的头文件
#define _BOX_SOURCE
#include "../libc/attr.h"
//一些东西
//是否有哪一位
//断言错误
extern "C"{
    //断言失败后调用
    BOX_NORETURN BOXAPI void _BoxAssertFail(
        const char *file,
        int line,
        const char *func,
        const char *exp
    );
    //启动线程
    //参数 第一个是启动函数 第二个是参数 第三个是C++ std::thread指针 可以为NULLPTR
    BOXAPI void _BoxStartThread(void(*start_fn)(void*),void *data,void *std_thread_ptr);
}
#ifndef BOX_ASSERT
    #ifndef NDEBUG
        #define BOX_ASSERT(EXP) \
            if(!(EXP)){\
                _BoxAssertFail(__FILE__,__LINE__,__FUNCTION__,#EXP)\
            }
    #else
        #define BOX_ASSERT(EXP) (EXP);
    #endif
#endif
#define BOX_HASBIT(INPUT,BIT) ((INPUT & BIT) == BIT)
#endif