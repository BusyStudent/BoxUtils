#include <new>
#include <cstdlib>
#include <cstring>
#include <exception>
#include "common/def.hpp"
#include "co/context.hpp"
#include "co/task.hpp"
#include "libc/mem.h"
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
    bool Context::swap(const Context &new_ctxt){
        return libc::swapcontext(this,&new_ctxt) == 0;
    }
    //得到当前上下文
    Context ThisContext() noexcept{
        Context ctxt;
        libc::getcontext(&ctxt);
        return ctxt;
    }
    void *AllocateStack(size_t size){
        void *ret;
        #ifdef _WIN32
            //在Window上以64对齐
            ret = libc::aligned_alloc(64,size);
        #else
            ret = malloc(size);
        #endif
        if(ret == nullptr){
            throw std::bad_alloc();
        }
        memset(ret,0,size);
        return ret;
    }
};
};

namespace Box{
namespace CoImpl{
    //当前任务
    thread_local void *current_task = nullptr;
};
};