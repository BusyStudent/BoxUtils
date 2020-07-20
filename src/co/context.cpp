#include <cstdlib>
#include "co/context.hpp"
namespace{
    using Box::Co::ContextEntry;
    void ucontext_wrapper(ContextEntry entry,ucontext_t *next,void *data){
        //调用函数
        if(entry != nullptr){
            entry(data);
        }
        if(next != nullptr){
            setcontext(next);
        }
    }
};

namespace Box{
namespace Co{
    //设置堆栈
    void Context::set_stack(void *ptr,size_t size){
        uc_stack.ss_size = size;
        uc_stack.ss_sp = ptr;
    }
    //设置进入点
    void Context::set_entry(ContextEntry entry,void *data){
        libc::makecontext(this,ucontext_wrapper,entry,uc_link,data);
    }
    //进入上下文
    bool Context::set() const{
        return libc::setcontext(this) == 0;
    }
    bool Context::operator()() const{
        return Context::set();
    }
    
    //得到当前上下文
    Context ThisContext() noexcept{
        Context ctxt;
        libc::getcontext(&ctxt);
        return ctxt;
    }
};
};