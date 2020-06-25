#include <cstring>
#include "ucontext.hpp"
#include "exception.hpp"
#ifdef _WIN32
//UContext-win32
extern "C"{
    #include "lib/ucontext.c"
}
#endif
#ifdef __ANDROID__
//Android上用libucontext的实现
extern "C"{
    //extern void setcontext();
}
#endif
namespace Box{
    typedef void(*Entry)(void*param);
    static void uconext_entry(Entry entry,ucontext_t *uc_link,void *param){
        //进入点
        entry(param);
        if(uc_link != nullptr){
            //切换到下一个
            setcontext(uc_link);
        }
    }
    UContext::UContext(){
        memset(this,0,sizeof(UContext));
        //得到一下
        this->get();
    }
    //设置进入点
    void UContext::set_entry(Entry entry,void *param){
        makecontext(this,(void(*)(void))uconext_entry,3,entry,uc_link,param);
    }
    void UContext::swap(UContext &new_context){
        swapcontext(this,&new_context);
        //切换一下
        Panic(nullptr);
    }
    void UContext::get(){
        getcontext(this);
    }
    void UContext::set(){
        //恢复过来
        setcontext(this);
        Panic(nullptr);
    }
};
