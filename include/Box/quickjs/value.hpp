#ifndef _BOX_QUICKJS_VALUE_HPP_
#define _BOX_QUICKJS_VALUE_HPP_
//数值
#include <quickjs/quickjs.h>
#include <string>
#include <cstddef>
#include <cstdint>
namespace Box{
    namespace QuickJS{
        //数值
        class Context;
        class Value{
            public:
                Value(Context &ctxt,const JSValue &v);
                Value(const Value &);
                ~Value();
                bool has_refcount() const noexcept;//是否有引用计数
                //是异常
                bool is_exception() const noexcept;
                //是错误
                bool is_error()     const noexcept;
                bool is_nan()       const noexcept;
                std::string to_string() const noexcept;//到字符串
                int get_count() const noexcept;//得到引用计数

                template <class T = JSValue> T get() const noexcept;//得到数据
                //得到属性
                Value operator [](const char *key) const noexcept;
                Value operator [](int index)       const noexcept;
                //添加属性
                bool add_item(uint32_t index ,const Value &v);//添加数值
                bool add_item(const char *key,const Value &v);//添加数值
                //是否还活着
                bool is_alive() const noexcept;
                Value &operator =(const Value &);//拷贝数值
                //输出 如果有错误
                void unwrap();
            private:
                Context &ctxt;
                JSValue value;
            friend class Context;
        };
        //得到int
        template<>
        int   Value::get<int >() const noexcept;
        //得到Bool
        template<>
        bool  Value::get<bool>() const noexcept;
        //得到double
        template<>
        double Value::get<double>() const noexcept;
        //得到指针
        template<>
        void* Value::get<void*>() const noexcept;
        //直接得到内部的value
        template<>
        JSValue Value::get<JSValue>() const noexcept{
            return value;
        }
    };
};
#endif