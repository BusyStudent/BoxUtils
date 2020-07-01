#include <functional>
#include <quickjs/quickjs.h>
#include "quickjs/runtime.hpp"
#include "quickjs/context.hpp"
namespace Box{
namespace QuickJS{
    Runtime::Runtime(){
        userdata = nullptr;
        //新建JS
        runtime = JS_NewRuntime();
        //绑定自己
        JS_SetRuntimeOpaque(runtime,this);
        //注册C++函数类
        func_id = 0;

        JS_NewClassID(&func_id);
        
    }
    Runtime::Runtime(Runtime &&rt){
        //移动
        userdata = rt.userdata;
        runtime = rt.runtime;
        rt.runtime = nullptr;

        func_id =  rt.func_id;
        //绑定一下
        JS_SetRuntimeOpaque(runtime,this);
    }
    Runtime::~Runtime(){
        if(runtime != nullptr){
            JS_FreeRuntime(runtime);
        }
    }
    //垃圾回收
    void Runtime::run_gc(){
        JS_RunGC(runtime);
    }
    Context Runtime::create_context(){
        return Context(*this);
    }
};
};