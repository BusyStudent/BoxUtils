#ifndef _BOX_LIBC_ATEXIT_H_
#define _BOX_LIBC_ATEXIT_H_
#include "attr.h"
#ifdef __cplusplus
extern "C"{
#endif

//在退出的时候调用函数
//nice越低越优先被调用
extern BOXAPI void Box_atexit(void(*fn)(),int nice);
//只会被加入一次
extern BOXAPI void Box_atexit_once(void(*fn)(),int nice);
extern BOXAPI void Box_atexit_ex(void(*fn)(void*),void *param,int nice);
//对于标准C的兼容函数
extern BOXAPI void __Box_stdc_handler(void *fn);

#ifdef __cplusplus
}
namespace Box{
    namespace libc{
        //C++版本
        inline void atexit(void(*fn)(),int nice = 0){
            Box_atexit(fn,nice);
        };
        inline void atexit_once(void(*fn)(),int nice = 0){
            Box_atexit_once(fn,nice);
        };
    };
} // namespace Box

#endif


#endif