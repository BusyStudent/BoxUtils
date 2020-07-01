#include <quickjs/quickjs.h>
#include "quickjs/value.hpp"
#include "quickjs/context.hpp"
#include "debug.hpp"
#include <stdexcept>
namespace Box{
namespace QuickJS{
    Value::Value(Context &ctxt,const JSValue &v):
        ctxt(ctxt),value(v){
        //产生一个新的 简单移动一下
    }
    Value::Value(const Value &v):
        ctxt(v.ctxt),value(JS_DupValue(v.ctxt.context,v.value)){
        //复制
    }
    Value::~Value(){
        //释放掉
        JS_FreeValue(ctxt.context,value);
    }
    //转换为字符串
    std::string Value::to_string() const noexcept{
        const char *cstr = JS_ToCString(ctxt.context,value);
        if(cstr == nullptr){
            //失败了
            return "[unknwon]";
        }
        else{
            std::string str(cstr);
            JS_FreeCString(ctxt.context,cstr);
            return str;
        }
    }
    //得到属性(不需要拷贝)
    Value Value::operator[](const char *key) const noexcept{
        return Value(
            ctxt,
            JS_GetPropertyStr(ctxt.context,value,key)
        );
    }
    Value Value::operator[](int index) const noexcept{
        return Value(
            ctxt,
            JS_GetPropertyUint32(ctxt.context,value,index)
        );
        
    }
    //添加数值(需要拷贝)
    bool Value::add_item(uint32_t index,const Value &v){
        return JS_SetPropertyUint32(ctxt.context,value,index,
            JS_DupValue(ctxt.context,v.value)
        ) == 0;
    }
    bool Value::add_item(const char* key,const Value &v){
        return JS_SetPropertyStr(ctxt.context,value,key,
            JS_DupValue(ctxt.context,v.value)
        ) == 0;
    }
    //赋值
    Value &Value::operator =(const Value &v){
        if(this != &v){
            if(ctxt.context != v.ctxt.context){
                //如果上下文不一样
                throw std::logic_error("value could not be assigned by which has different context");
            }
            //释放自己掉 再拷贝
            JS_FreeValue(ctxt.context,value);
            value = JS_DupValue(ctxt.context,v.value);
        }
        return *this;
    }
    //得到数据
    bool Value::has_refcount() const noexcept{
        return JS_VALUE_HAS_REF_COUNT(value);
    }
    //是异常
    bool Value::is_exception() const noexcept{
        return JS_IsException(value);
    }
    bool Value::is_error() const noexcept{
        return JS_IsError(ctxt.context,value);
    }
    bool Value::is_nan()   const noexcept{
        return JS_VALUE_IS_NAN(value);
    }
    //输出 如果有错误
    void Value::unwrap(){
        if(is_exception()){
            Value exp = ctxt.get_exception();
            fprintf(stderr,"\033[33mException: \033[0m%s\n",exp.to_string().c_str());
            fprintf(stderr,"%s\n",exp["stack"].to_string().c_str());
        }
    }
    //是否或者
    bool Value::is_alive() const noexcept{
        return JS_IsLiveObject(JS_GetRuntime(ctxt.context),value);
    }
    //简体啊
    //得到引用计数
    int Value::get_count() const noexcept{
        if(JS_VALUE_HAS_REF_COUNT(value)){
            JSRefCountHeader *header = static_cast<JSRefCountHeader*>(JS_VALUE_GET_PTR(value));
            return header->ref_count;
        }
        return -1;
    }

    template<>
    bool Value::get<bool>() const noexcept{
        return JS_VALUE_GET_BOOL(value);
    }
    template<>
    double Value::get<double>() const noexcept{
        return JS_VALUE_GET_FLOAT64(value);
    }
    template<>
    int Value::get<int>() const noexcept{
        return JS_VALUE_GET_INT(value);
    }
    template<>
    void* Value::get<void*>() const noexcept{
        return JS_VALUE_GET_PTR(value);
    }
};
};