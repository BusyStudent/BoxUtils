#ifndef _BOX_QUICKJS_RUNTIME_HPP_
#define _BOX_QUICKJS_RUNTIME_HPP_
//简单QuickJS封装
#include <functional>
#include <cstdint>
struct JSRuntime;
namespace Box{
    namespace QuickJS{
        //运行时
        class Context;
        class Runtime{
            public:
                typedef std::function<bool()> InterruptHandler;
                Runtime();
                Runtime(Runtime &&);
                ~Runtime();
                void run_gc();//垃圾回收
                Context create_context();//创建上下文
                //内存函数
                void *malloc();
                void  free();
            public:
                //得到runtime
                JSRuntime* get() const noexcept{
                    return runtime;
                }
                explicit operator JSRuntime*() const noexcept{
                    return runtime;
                }
                uint32_t get_func_id() const noexcept{
                    return func_id;
                }
            private:
                JSRuntime *runtime;
                //用户数据
                void *userdata;
                //函数的ID
                uint32_t  func_id;
            friend class Context;
        };
    };
};
#endif