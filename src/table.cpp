#include <unordered_map>
#include <typeinfo>
#include <string>
#include <list>
#include "common/def.hpp"
#include "table.hpp"
#include "backtrace.hpp"
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
//AnyObject
namespace Box{
    AnyObject::AnyObject(){
        ptr = nullptr;
        delete_fn = nullptr;
        typeinfo = nullptr;
    }
    //移动
    AnyObject::AnyObject(AnyObject &&obj){
        ptr = obj.ptr;
        delete_fn = obj.delete_fn;
        typeinfo = obj.typeinfo;
        //清除原来的
        obj.ptr = nullptr;
        obj.delete_fn = nullptr;
        obj.typeinfo = nullptr;
    }
    //字符串特殊话初始化
    AnyObject::AnyObject(const char *str){
        ptr = new std::string(str);
        delete_fn = AnyImpl::DeleteFn<std::string>;
        typeinfo = &typeid(std::string);
    }
    AnyObject::~AnyObject(){
        //销毁掉
        if(ptr != nullptr){
            BOX_ASSERT(delete_fn != nullptr);
            delete_fn(ptr);
        }
    }

    //检查类型 读取指针
    const void *AnyObject::load_ptr(const std::type_info &info) const{
        //const版本
        //空的
        if(ptr == nullptr or typeinfo == nullptr){
            throwNullPtrException();
        }
        if(*typeinfo == info){
            //类型符合
            return ptr;
        }
        //类型不符合
        throw TypeError(
            Backtrace::Demangle(info.name()).c_str(),
            Backtrace::Demangle(typeinfo->name()).c_str()
        );
    }
    void *AnyObject::load_ptr(const std::type_info &info){
        //非const版本
        if(ptr == nullptr or typeinfo == nullptr){
            throwNullPtrException();
        }
        if(*typeinfo == info){
            //类型符合
            return ptr;
        }
        //类型不符合
        throw TypeError(
            Backtrace::Demangle(info.name()).c_str(),
            Backtrace::Demangle(typeinfo->name()).c_str()
        );
    }


    //类型
    const std::type_info &AnyObject::type() const{
        if(typeinfo == nullptr){
            return typeid(void);
        }
        return *typeinfo;
    }
    //类型名字
    std::string AnyObject::type_name() const{
        return Backtrace::Demangle(type().name());
    }
    //重置
    void AnyObject::reset(){
        if(ptr != nullptr){
            delete_fn(ptr);
        }
        //清空
        ptr = nullptr;
        delete_fn = nullptr;
        typeinfo = nullptr;
    }
    //字符串的特殊化
    AnyObject &AnyObject::operator =(const char *str){
        if(ptr != nullptr){
            //销毁原来数据
            delete_fn(ptr);
        }
        ptr = new std::string(str);
        delete_fn = AnyImpl::DeleteFn<std::string>;
        typeinfo = &typeid(std::string);
        return *this;
    }
    //方便 使用 List和 Table的接口
    void AnyObject::insert(const std::string &key,AnyObject && obj){
        cast<AnyTable&>().insert(key,std::forward<AnyObject>(obj));
    }
    void AnyObject::insert(const std::string &key,AnyObject * obj){
        cast<AnyTable&>().insert(key,obj);
    }
    //查找键
    const AnyObject &AnyObject::operator [](const std::string &key) const{
        return cast<const AnyTable&>()[key];
    }
    AnyObject &AnyObject::operator [](const std::string &key){
        return cast<AnyTable&>()[key];
    }
    //查找列表
    const AnyObject &AnyObject::operator [](int index) const{
        return cast<const AnyList&>()[index];
    }
    AnyObject &AnyObject::operator [](int index){
        return cast<AnyList&>()[index];
    }
}
//AnyTable
namespace Box{
    AnyTable::AnyTable(){}
    AnyTable::AnyTable(AnyTable &&t):
        table(std::move(t.table))
    {
        //移动表
    }
    AnyTable::~AnyTable(){
        AnyTable::clear();
    }
    void AnyTable::clear(){
        for(auto &object:table){
            //删除所有数据
            delete object.second;
        }
        //清空
        table.clear();
    }
    void AnyTable::insert(const std::string &key,AnyObject &&obj){
        return AnyTable::insert(key,new AnyObject(std::move(obj)));
    }
    //插入一个键
    void AnyTable::insert(const std::string &key,AnyObject *obj){
        std::map<std::string,AnyObject*> ::iterator iter = table.find(key);
        if(iter == table.end()){
            //没找到
            table.emplace(std::make_pair(key,obj));
            //插入一个
        }
        else{
            //删除原来的
            delete iter->second;
            //移动到上面
            iter->second = obj;
        }
    }
    //查找键值
    const AnyObject &AnyTable::operator [](const std::string &key) const{
        std::map<std::string,AnyObject*> ::const_iterator iter = table.find(key);
        //查表
        if(iter == table.end() or iter->second == nullptr){
            //没找到
            throwKeyError(key.c_str());
        }
        return *(iter->second);
    }
    AnyObject &AnyTable::operator [](const std::string &key){
        std::map<std::string,AnyObject*> ::iterator iter = table.find(key);
        //查表
        if(iter == table.end() or iter->second == nullptr){
            //没找到
            throwKeyError(key.c_str());
        }
        return *(iter->second);
    }
    //移除键值
    bool AnyTable::remove(const std::string &key){
        std::map<std::string,AnyObject*> :: iterator iter = table.find(key);
        if(iter == table.end() or iter->second == nullptr){
            //失败
            return false;
        }
        //删除数据
        delete iter->second;
        iter->second = nullptr;
        table.erase(iter);
        return true;
    }
    //是否有键值
    bool AnyTable::has_key(const std::string &key) const noexcept{
        return table.count(key) == 1;
    }
    //弹出数值
    AnyObject *AnyTable::pop(const std::string &key) noexcept{
        std::map<std::string,AnyObject *> ::iterator iter = table.find(key);
        if(iter == table.end()){
            //失败
            return nullptr;
        }
        AnyObject *ptr = iter->second;
        iter->second = nullptr;
        table.erase(iter);
        return ptr;
    }
    //移动一下
    AnyTable &AnyTable::operator =(AnyTable &&t){
        if(&t != this){
            AnyTable::clear();
            table = std::move(t.table);
        }
        return  *this;
    }
}
//AnyList
namespace Box{
    AnyList::AnyList(std::initializer_list<AnyObject> &&vlist){
        //从列表初始化
        for(auto &obj:vlist){
            list.push_back(new AnyObject(std::move(const_cast<AnyObject&>(obj))));
        }
    }
    AnyList::AnyList(){}
    AnyList::~AnyList(){
        for(auto &obj:list){
            delete obj;
        }
    }
    void AnyList::push_back(AnyObject &&obj){
        list.push_back(new AnyObject(std::move(obj)));
    }
    void AnyList::push_front(AnyObject &&obj){
        list.push_back(new AnyObject(std::move(obj)));
    }
    //查列表
    //const 版本
    const AnyObject &AnyList::operator [](int index) const{
        if(index < 0){
            //反向查找
            if( ((-index) - 1) >= (int)list.size()){
                //越界
                throw IndexError(index);
            }
            std::list<AnyObject*> ::const_reverse_iterator iter = list.rbegin();
            for(int i = -1; i >  index; i --){
                ++iter;
            }
            //找到了
            return **iter;
        }
        else{
            if(index >= (int)list.size()){
                throw IndexError(index);
            }
            std::list<AnyObject*> ::const_iterator iter = list.begin();
            for(int i = 0;i < index; i ++){
                ++iter;
            }
            return **iter;
        }
    }
    //非const版本
    AnyObject &AnyList::operator [](int index){
        if(index < 0){
            //反向查找
            if( ((-index) - 1) >= (int)list.size()){
                //越界
                throw IndexError(index);
            }
            std::list<AnyObject*> ::reverse_iterator iter = list.rbegin();
            for(int i = -1; i >  index; i --){
                ++iter;
            }
            //找到了
            return **iter;
        }
        else{
            if(index >= (int)list.size()){
                throw IndexError(index);
            }
            std::list<AnyObject*> ::iterator iter = list.begin();
            for(int i = 0;i < index; i ++){
                ++iter;
            }
            return **iter;
        }
    }
}