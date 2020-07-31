#include <unordered_map>
#include <typeinfo>
#include <string>
#include <list>
#include "table.hpp"
#include "assert.hpp"
#include "exception.hpp"
//创建销毁函数
#define CREATE_DELETE_FN(TYPE) \
    template<>\
    BOXAPI void Box::AnyImpl::DeleteFn<TYPE>(void *ptr){\
        delete static_cast<TYPE*>(ptr);\
    }
//创建typeid的函数
#define CREATE_TYPE_INFO(TYPE) \
    template<>\
    BOXAPI const std::type_info *Box::AnyImpl::TypeInfo<TYPE>(){\
        return &typeid(TYPE);\
    }
//两个一起创建
#define CREATE_TYPE_FN_BOTH(TYPE) \
    CREATE_TYPE_INFO(TYPE);\
    CREATE_DELETE_FN(TYPE);
//内置的销毁函数
namespace Box{
    //AnyObject
    //销毁Holder里面数据
    AnyObject::Holder::~Holder(){
        BOX_ASSERT(delete_fn != nullptr);
        //销毁器
        delete_fn(ptr);
    }
    AnyObject::~AnyObject(){
        delete holder;
    }
    const std::type_info &AnyObject::type() const{
        if(holder == nullptr){
            throwNullPtrException();
        }
        return *(holder->type);
    }
    //一些类型
    //常用销毁函数
    CREATE_TYPE_FN_BOTH(int);
    CREATE_TYPE_FN_BOTH(bool);
    CREATE_TYPE_FN_BOTH(float);
    CREATE_TYPE_FN_BOTH(double);
    CREATE_TYPE_FN_BOTH(AnyList);
    CREATE_TYPE_FN_BOTH(AnyTable);
    CREATE_TYPE_FN_BOTH(std::string);
}
namespace Box{
    AnyTable::~AnyTable(){

    }
    AnyList::~AnyList(){

    }
    //查表
    const AnyObject &AnyTable::operator [](const std::string &key) const{
        std::map<std::string,AnyObject> ::const_iterator iter;
        iter = table.find(key);
        if(iter == table.end()){
            //失败
            throwKeyError(key.c_str());
        }
        else{
            return iter->second;
        }
    }
    AnyObject &AnyTable::operator [](const std::string &key){
        std::map<std::string,AnyObject> ::iterator iter;
        iter = table.find(key);
        if(iter == table.end()){
            //失败
            throwKeyError(key.c_str());
        }
        else{
            return iter->second;
        }
    }
    bool AnyTable::has_key(const std::string &key) const noexcept{
        return table.count(key) == 1;
    }
};