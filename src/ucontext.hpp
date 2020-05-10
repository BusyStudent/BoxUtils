//UContext封装
#ifndef _BOX_UCONTEXT_HPP_
#define _BOX_UCONTEXT_HPP_
#include <ucontext.h>
namespace Box{
    struct UContext:public ucontext_t{
        UContext();//初始化清空
        void get();//保存当前的状态
        void set_entry(void(*entry)(void*param),void *param);//设置进入点
        void swap(UContext &new_context);//和另一个交换数据
        void set();//恢复到这个上下文中
    };
};
#endif