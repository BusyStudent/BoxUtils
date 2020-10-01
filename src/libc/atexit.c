#define _BOX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "libc/atexit.h"
#include "libc.h"
//定义
LIBC_BEGIN
struct BoxExitHandler{
    //退出的处理器
    void (*fn)(void*);//函数
    void *param;//参数
    int nice;//优先级
    struct BoxExitHandler *next;
};
typedef struct BoxExitHandler BoxExitHandler;
BOXAPI void __Box_stdc_handler(void *fn){
    //对于标准C的兼容函数
    typedef void(*stdc_handler)();
    ((stdc_handler)fn)();
};
static BoxExitHandler *exit_handlers = nullptr;//退出的函数链表
#if !defined(BOX_LIBC_NTHSAFE)
    //线程安全
    //#include <threads.h>
    static libc_mutex_t handlers_mtx;
    //初始化锁
    #ifdef __GNUC__
    static BOX_CONSTRUCTOR void atexit_init_mtx(){
        LIBC_MUTEX_INIT(handlers_mtx);
    };
    #elif defined(_MSC_VER)
    //MSVC上提前调用
    #pragma data_seg(".CRT$XIU")   
    static void atexit_init_mtx(){
        LIBC_MUTEX_INIT(handlers_mtx);
    };
    #pragma data_seg()
    #else
        #error "Donot support it please use -DBOX_LIBC_NTHSAFE"
    #endif
    #define GLOBAL_LOCK() LIBC_MUTEX_LOCK(handlers_mtx)
    #define GLOBAL_UNLOCK() LIBC_MUTEX_UNLOCK(handlers_mtx)
    #define MUTEX_CLEANUP() LIBC_MUTEX_DESTROY(handlers_mtx)
#else
    //忽略的
    #define GLOBAL_LOCK()
    #define GLOBAL_UNLOCK()
    #define MUTEX_CLEANUP()
#endif
static void __Box_atexit_ex_impl(void(*fn)(void*),void *param,int nice);
static void call_handlers(){
    //调用一下函数
    GLOBAL_LOCK();
    BoxExitHandler *current = exit_handlers;
    BoxExitHandler *temp;
    
    while(current != nullptr){
        current->fn(current->param);
        temp = current->next;
        free(current);
        current = temp;
    }
    GLOBAL_UNLOCK();
    //清理锁
    MUTEX_CLEANUP();
};


BOXAPI void Box_atexit(void(*fn)(),int nice){
    return Box_atexit_ex(__Box_stdc_handler,fn,nice);
}
BOXAPI void Box_atexit_once(void(*fn)(),int nice){
    GLOBAL_LOCK();
    for(BoxExitHandler *current = exit_handlers;current != nullptr;current = current->next){
        //查找有没有相同的
        if(current->fn == __Box_stdc_handler and current->param == fn){
            //有了
            GLOBAL_UNLOCK();
            return;
        }
    }
    __Box_atexit_ex_impl(__Box_stdc_handler,fn,nice);
    GLOBAL_UNLOCK();
}
BOXAPI void Box_atexit_ex(void(*fn)(void*),void *param,int nice){
    GLOBAL_LOCK();
    __Box_atexit_ex_impl(fn,param,nice);
    GLOBAL_UNLOCK();
}
//真正的实现
static void __Box_atexit_ex_impl(void(*fn)(void*),void *param,int nice){
    if(fn == nullptr){
        //检查一下
        return;
    }
    if(exit_handlers == nullptr){
        //没初始化过
        atexit(call_handlers);
        //加入一个
        exit_handlers = malloc(sizeof(BoxExitHandler));
        exit_handlers->fn = fn;
        exit_handlers->param = param;
        exit_handlers->nice = nice;
        exit_handlers->next = nullptr;
        return;
    }
    //获取当前的
    BoxExitHandler *current = exit_handlers;
    BoxExitHandler *prev = nullptr;
    while(true){
        //符合优先级别 或者是最后一个
        if(current->nice >= nice or current->next == nullptr){
            //插入在这里
            BoxExitHandler *hd = malloc(sizeof(BoxExitHandler));
            hd->fn = fn;
            hd->param = param;
            hd->nice = nice;
            hd->next = current;//设置一下个
            if(prev != nullptr){
                prev->next = hd;//设置前一个
            }
            return;
        }
        else{
            //前往下一个
            prev = current;
            current = current->next;
        }
    }
}
LIBC_END