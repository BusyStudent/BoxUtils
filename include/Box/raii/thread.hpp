#if !defined(_BOX_RAII_THREAD_HPP_)
#define _BOX_RAII_THREAD_HPP_
#include <thread>
#include <mutex>
#define Box_callonce static Box::RAII::OnceInvoker __ONCE_INVOKER = [&]()
#define Box_onload   static Box::RAII::OnloadInvoker __ONLOAD_INVOKER = [ ]()
#define Box_onexit   static Box::RAII::OnExitInvoker __ONEXIT_INVOKER = [&]()
namespace Box{
    namespace RAII{
        class AtThreadExit{
            
        };
        //调用一次
        template<class T>
        class OnceInvoker{
            public:
                OnceInvoker(T &&fn):flags(){
                    std::call_once(flags,std::forward<T>(fn));
                };
                OnceInvoker(const OnceInvoker &) = delete;
                ~OnceInvoker(){}
            private:
                std::once_flag flags;
        };
        //在最开始加载
        template<class T>
        class OnloadInvoker{
            public:
                OnloadInvoker(T &&fn){
                    fn();
                }
                
        };
        //在结束时候
        template<class T>
        class OnExitInvoker{
            public:
                OnExitInvoker(T &&f):fn(f){}
                OnExitInvoker(const OnExitInvoker &) = delete;
                ~OnExitInvoker(){
                    fn();
                }
            private:
                T fn;
        };
    };
};


#endif // _BOX_RAII_THREAD_HPP_
