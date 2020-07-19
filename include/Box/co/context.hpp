#ifndef _BOX_CO_CONTEXT_HPP_
#define _BOX_CO_CONTEXT_HPP_

#if defined(_WIN32)
    #warning 
#else
    #include <ucontext.h>
#endif

#ifdef _ANDROID_
//Android上用libucontext
extern "C"{
    extern int getcontext (ucontext_t *uctxt);
    extern int setcontext (const ucontext_t *uctxt);
    extern int swapcontext (ucontext_t * ouctxt,const ucontext_t * nuctxt);
    extern void makecontext (ucontext_t *uctxt, void (*fn) (void),int argc, ...);

};
#endif
namespace Box{
    namespace libc{
        using ::ucontext_t;
        using ::getcontext;
        using ::setcontext;
        using ::swapcontext;
        //这个要小心一点
        template<class T,class ...Args>
        void makecontext(ucontext_t *uctxt,const T&fn,Args &&...args){
            ::makecontext(uctxt,reinterpret_cast<void(*)()>(fn),sizeof...(Args),args...);
        };
    };
    namespace Co{
        typedef void(*ContextEntry)(void*);
        struct Context:public libc::ucontext_t{
            //设置入口点
            void set_entry(ContextEntry,void *data = nullptr);
            //切换进入这个上下文
            bool set() const;
            bool operator ()() const;
        };
        //当前的上下文
        Context ThisContext() noexcept;
    };
};


#endif