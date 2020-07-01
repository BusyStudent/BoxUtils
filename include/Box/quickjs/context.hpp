#ifndef _BOX_QUICKJS_HPP_
#define _BOX_QUICKJS_HPP_
#include <string>
#include <cstddef>
#include <functional>
struct JSContext;
struct JSValue;
namespace Box{
    
    class Json;
    namespace QuickJS{
        class Value;
        class Runtime;
        class Context;
        namespace EvalFlags{
            //各种解析数值
            extern const int GLOBAL;//全局(默认)
            extern const int MOUDLE;//模块
            extern const int DIRECT;//直接调用
            extern const int INDIRECT;//不直接
            extern const int STRICY;//严格模式
            extern const int STRIP;
            extern const int COMPILE_ONLY;//只是编译
            extern const int BACKTRACE_BARRIER;//不显示在回溯里面
        };
        typedef JSValue (*CFunction)(JSContext*,JSValue self,int argc,JSValue argv[]);//C函数 没啥封装
        typedef std::function <Value(Context&,Value self,int argc,Value argv[])> Function;//包装过的函数
        class Context{
            public:
                Context(Runtime  &);//从Runtime构建
                Context(Context &&);//移动
                Context(const Context &);//复制
                ~Context();
                //解释字符串
                Value eval(const char *str,int len,int flags = EvalFlags::GLOBAL);
                Value eval(const std::string &str,int flags = EvalFlags::GLOBAL);
                //解释文件
                Value eval_file(const char *file       ,int flags = EvalFlags::GLOBAL);
                Value eval_file(const std::string &file,int flags = EvalFlags::GLOBAL);
                //得到全局对象
                Value global_object() const;
                Value get_exception() const;//得到异常

                Value parse_json(const std::string &str);//解析Json
                Value from(const Json &);//转换对象到数值

                Runtime &runtime() const;//得到所属于的Runtime
                //创建对象
                Value new_bool(bool val) noexcept;
                Value new_object() noexcept;
                Value new_undefined() noexcept;//未定义对象
                //C函数
                Value new_function(const char *name,CFunction);
                //C++函数 包装了一下(有点不稳定)
                Value new_function(const char *name,const Function&) noexcept;
                //启用内建的一下功能
                void add_base_objects() noexcept;//基本对象
                void add_operators()    noexcept;//操作符
                void add_console()      noexcept;//启用终端
                void add_eval() noexcept;//Eval
                //内存函数
                void *malloc(size_t n) noexcept;
                void *realloc(void *ptr,size_t new_size) noexcept;
                void  free(void *ptr) noexcept;
            public:
                JSContext *get() const noexcept{
                    return context;
                }
                //内存函数模板
                template<class T>
                T *malloc(size_t n) noexcept{
                    return static_cast<T*>(Context::malloc(n));
                }
                template<class T>
                void free(T *ptr) noexcept{
                    Context::free(ptr);
                }
                template<class T>
                T *realloc(T *ptr,size_t n) noexcept{
                    return static_cast<T*>(Context::realloc(ptr,n));
                }
            private:
                JSContext *context;
            friend class Value;
        };
    };
};
#endif