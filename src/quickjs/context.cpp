#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>
#include "quickjs/context.hpp"
#include "quickjs/runtime.hpp"
#include "quickjs/value.hpp"
#include "exception.hpp"
#include "backtrace.hpp"
#include "json.hpp"
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <typeinfo>

namespace Box{
namespace QuickJS{
    extern const JSClassDef func_def;
    Context::Context(Runtime &rt){
        //创建
        context = JS_NewContext(rt.runtime);
        //绑定自己
        JS_SetContextOpaque(context,this);
        JS_NewClass(rt.runtime,rt.func_id,&func_def);
    }
    Context::Context(Context &&ctxt){
        context = ctxt.context;
        ctxt.context = nullptr;
        //绑定自己
        JS_SetContextOpaque(context,this);
    }
    Context::Context(const Context &ctxt){
        context = JS_DupContext(ctxt.context);
        //绑定自己
        JS_SetContextOpaque(context,this);
    }
    Context::~Context(){
        if(context != nullptr){
            JS_FreeContext(context);
        }
    }
    //得到runtime
    Runtime &Context::runtime() const{
        Runtime *rt = static_cast<Runtime*>(JS_GetRuntimeOpaque(
            JS_GetRuntime(context)
        ));
        if(rt == nullptr){
            //如果没有的话
            throw Box::NullPtrException();
        }
        return *rt;
    }
    //解释字符串
    Value Context::eval(const std::string &str,int flags){
        return Value(*this,
            JS_Eval(context,str.c_str(),str.length() + 1,"<Context::eval()>",flags)
        );
    }
    //解释文件
    Value Context::eval_file(const char *file,int flags){
        //读入
        std::ifstream stream(file);
        std::istreambuf_iterator<char> begin(stream),end;
        std::string str(begin,end);
        if(stream.bad()){
            throw FileNotFoundError(errno,file);
        }
        stream.close();
        //解析
        return Value(
            *this,
            JS_Eval(context,str.c_str(),str.length() + 1,file,flags)
        );
    }
    Value Context::eval_file(const std::string &file,int flags){
        return eval_file(file.c_str(),flags);
    }
    //创建对象
    Value Context::new_object() noexcept{
        return Value(
            *this,
            JS_NewObject(context)
        );
    }
    Value Context::new_bool(bool val) noexcept{
        return Value(
            *this,
            JS_NewBool(context,val)
        );
    }
    //C函数
    Value Context::new_function(const char *name,CFunction func){
        if(func == nullptr){
            throw NullPtrException();
        }
        return Value(
            *this,
            JS_NewCFunction(context,func,name,1)
        );
    }
    //C++函数的定义
    struct CxxFunction;
    typedef JSValue (*CxxFuncCall)(CxxFunction&fn,Context &,JSValueConst func_obj,
                      JSValueConst this_val, int argc, JSValueConst *argv);
    typedef void (*CxxFuncDel)(CxxFunction &);
    struct CxxFunction{
        
        CxxFunction(CxxFuncDel del,CxxFuncCall cal,void *data){
            userdata = data;
            desturcotr = del;
            call = cal;
        }
        ~CxxFunction(){
            if(desturcotr != nullptr){
                //调用函数
                desturcotr(*this);
            }
        }
        std::vector<Value> values;//参数们
        void *userdata;
        CxxFuncCall call;
        void (*desturcotr)(CxxFunction &);//销毁
    };
    const JSClassDef func_def = {
        .class_name = "QuickJSCxxFunction",
        .finalizer = [](JSRuntime *rt,JSValue self){
            JSClassID id = static_cast<Runtime*>(JS_GetRuntimeOpaque(rt))->get_func_id();
            //得到class id
            CxxFunction *fn = static_cast<CxxFunction*>(JS_GetOpaque(self,id));
            //得到函数 销毁
            delete fn;
        },
        .gc_mark = nullptr,
        .call = [](JSContext *context, JSValueConst func,
                    JSValueConst self, int argc, JSValueConst *argv,
                    int flags)  -> JSValue{
            //调用
            if((flags & JS_CALL_FLAG_CONSTRUCTOR) != 0){
                //抛出异常
                return JS_Throw(
                    context,
                    JS_NewString(context,"[CxxFunction]This object cannot be called as a constructor")
                );
            }
            Context *ctxt = static_cast<Context*>(JS_GetContextOpaque(context));
            JSClassID id = ctxt->runtime().get_func_id();
            //得到ID
            CxxFunction *fn = static_cast<CxxFunction*>(JS_GetOpaque(func,id));
            //得到fn
            if(fn != nullptr){
                try{
                    return fn->call(*fn,*ctxt,func,self,argc,argv);
                }
                catch(std::exception &except){
                    //捕获标准异常 转换一下
                    return JS_ThrowInternalError(context,"%s::'%s'",
                        Backtrace::Demangle(typeid(except).name()).c_str(),
                        except.what()
                    );
                }
            }
            else{
                #ifndef NDEBUG
                Backtrace::Show();
                #endif
                return JS_Throw(
                    context,
                    JS_NewString(context,"[CxxFunction]This object cannot be called")
                );
            }
        },
        .exotic = nullptr
    };
    //C++包装
    Value Context::new_function(const char *name,const Function &cxxfn) noexcept{
        //name暂时没实现
        static thread_local std::stringstream sstream;//字符串流
        CxxFunction *fn =  new CxxFunction(
            [](CxxFunction &fn){
                //删除数据
                delete static_cast<Function*>(fn.userdata);
            },
            []( CxxFunction&fn,Context &ctxt,JSValueConst func,
                JSValueConst, int argc, JSValueConst *argv) -> JSValue{
                //调用函数
                fn.values.clear();//清空vector
                Function *f = static_cast<Function*>(fn.userdata);
                
                //拷贝到vector
                for(int i = 0;i < argc; i ++){
                    fn.values.push_back(
                        Value(
                            ctxt,
                            JS_DupValue(ctxt.get(),argv[i])
                        )
                    );
                }

                Value v =  (*f)(ctxt,
                    Value(ctxt,func),
                    argc,
                    fn.values.data()
                );
                //拷贝返回
                return JS_DupValue(ctxt.get(),v.get());
            },
            new Function(cxxfn)
        );
        JSClassID id = runtime().func_id;
        JSValue v = JS_NewObjectClass(context,id);
        JS_SetOpaque(v,fn);
        //创建atom 来添加属性
        JS_SetPropertyStr(
            context,
            v,
            "toString",
            JS_NewCFunction(
                context,[](JSContext *ctxt,JSValueConst self,int argc,JSValueConst *) -> JSValue{
                    //得到函数描述
                    if(argc != 0){
                        //多个参数
                        return JS_ThrowSyntaxError(ctxt,"method toString() only take 0 args");
                    }
                    return JS_GetPropertyStr(ctxt,self,"__function__");
                },"toString",1
            )
        );
        //格式化名字
        sstream.clear();
        sstream.str("");
        //格式化toString()的结果
        sstream << "function " << name << "(){\n    [native code]\n}";
        JS_SetPropertyStr(context,v,"name",JS_NewString(context,name));
        JS_SetPropertyStr(context,v,"__function__",JS_NewString(context,sstream.str().c_str()));
        //创建数值
        ///note 蜜汁BUG Dup后不调用函数出现第1个错误 
        //assertion list_empty(&rt->gc_obj_list) failed
        //Assertion `p->ref_count > 0' failed
        return Value(
            *this,
            v
        );
    }
    //启用内建功能
    //启用基本对象
    void Context::add_base_objects() noexcept{
        JS_AddIntrinsicBaseObjects(context);
    }
    //启用操作符
    void Context::add_operators() noexcept{
        JS_AddIntrinsicOperators(context);
    }
    //启用终端(console.log)
    void Context::add_console() noexcept{
        JSValue console = JS_NewObject(context);
        JSValue global = JS_GetGlobalObject(context);
        JS_SetPropertyStr(context,global,"console",console);
        //添加函数到console
        JS_SetPropertyStr(context,console,"log",
            JS_NewCFunction(context,
                [](JSContext *context,JSValueConst,
                   int argc, JSValueConst *argv) -> JSValue{
                //输出函数
                static std::mutex mtx;//锁
                std::lock_guard<std::mutex> locker(mtx);
                const char *str;
                for(int i = 0;i < argc; i++){
                    if(i != 0){
                        //加个空格
                        fputc(' ',stdout);
                    }
                    str = JS_ToCString(context,argv[i]);
                    if(str == nullptr){
                        //失败
                        return JS_EXCEPTION;
                    }
                    //输出
                    fputs(str,stdout);
                    //释放
                    JS_FreeCString(context,str);
                }
                //换行
                fputc('\n',stdout);
                return JS_UNDEFINED;
            },"log",1)
        );

        //释放global
        JS_FreeValue(context,global);
    }
    //启用Eval
    void Context::add_eval() noexcept{
        JS_AddIntrinsicEval(context);
    }
    //内存函数
    void *Context::malloc(size_t n) noexcept{
        return js_malloc(context,n);
    }
    void *Context::realloc(void *ptr,size_t n) noexcept{
        return js_realloc(context,ptr,n);
    }
    void  Context::free(void *ptr) noexcept{
        return js_free(context,ptr);
    }
    //得到全局对象
    Value Context::global_object() const{
        return Value(
            *const_cast<Context*>(this),
            JS_GetGlobalObject(context)
        );
    }
    //得到异常
    //复制一下
    Value Context::get_exception() const{
        return Value(
            *const_cast<Context*>(this),
            JS_GetException(context)
        );
    }
    //解析Json
    Value Context::parse_json(const std::string &str){
        return Value(
            *this,
            JS_ParseJSON(context,str.c_str(),str.length() + 1,"<JSON>")
        );
    }
    //转换对象
    Value Context::from(const Json &json){
        return parse_json(json.to_string());
    }
    namespace EvalFlags{
        const int GLOBAL = JS_EVAL_TYPE_GLOBAL;
        const int MOUDLE = JS_EVAL_TYPE_MODULE;
        const int DIRECT = JS_EVAL_TYPE_DIRECT;
        const int STRICT = JS_EVAL_FLAG_STRICT;
        const int STRIP = JS_EVAL_FLAG_STRIP;
        const int INDIRECT = JS_EVAL_TYPE_INDIRECT;
        const int COMPILE_ONLY = JS_EVAL_FLAG_COMPILE_ONLY;
        const int BACKTRACE_BARRIER = JS_EVAL_FLAG_BACKTRACE_BARRIER;
    };
};
};