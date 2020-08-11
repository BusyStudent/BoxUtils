#define _BOX_SOURCE
#include <stdlib.h>
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
#if defined(BOX_LIBC_THSAFE) && !defined(__STDC_NO_THREADS__)
    //线程安全
    #include <threads.h>
    static mtx_t handlers_mtx;
    //删除锁
    static void delete_mtx(void *mutex){
        mtx_destroy((mtx_t*)mutex);
    };
#endif
static void call_handlers(){
    //调用一下函数
    BoxExitHandler *current = exit_handlers;
    BoxExitHandler *temp;
    
    while(current != nullptr){
        current->fn(current->param);
        temp = current->next;
        free(current);
        current = temp;
    }
};


BOXAPI void Box_atexit(void(*fn)(),int nice){
    return Box_atexit_ex(__Box_stdc_handler,fn,nice);
}
BOXAPI void Box_atexit_once(void(*fn)(),int nice){
    for(BoxExitHandler *current = exit_handlers;current != nullptr;current = current->next){
        //查找有没有相同的
        if(current->fn == __Box_stdc_handler and current->param == fn){
            //有了
            return;
        }
    }
    return Box_atexit_ex(__Box_stdc_handler,fn,nice);
}
//真正的实现
BOXAPI void Box_atexit_ex(void(*fn)(void*),void *param,int nice){
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