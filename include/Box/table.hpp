#ifndef _BOX_TABLE_HPP_
#include <map>
#include <list>
#include <string>
#include <typeinfo>
#include <type_traits>
#include "libc/attr.h"
namespace Box{
    //内部一些常量
    class AnyObject;
    class AnyTable;
    class AnyList;
    namespace AnyImpl{
        //常用类型
        //得到TypeInfo的函数
        template<class T>
        const std::type_info* TypeInfo(){
            return &typeid(T);
        }
        
        template<>
        BOXAPI const std::type_info* TypeInfo<int>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<bool>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<float>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<double>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<AnyList>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<AnyTable>();
        
        template<>
        BOXAPI const std::type_info* TypeInfo<std::string>();

        //销毁函数模板
        template<class T>
        BOXAPI void DeleteFn(void *ptr){
            delete static_cast<T*>(ptr);
        };
        
        template<>
        BOXAPI void DeleteFn<int>(void *ptr);
        
        template<>
        BOXAPI void DeleteFn<bool>(void *ptr);
        
        template<>
        BOXAPI void DeleteFn<float>(void *ptr);
        
        template<>
        BOXAPI void DeleteFn<double>(void *ptr);
        
        template<>
        BOXAPI void DeleteFn<AnyList>(void *ptr);
        
        template<>
        BOXAPI void DeleteFn<AnyTable>(void *ptr);

        template<>
        BOXAPI void DeleteFn<std::string>(void *ptr);
    };
    //可以容纳各种类型的表 像Python dict
    class BOXAPI AnyObject{
        //像any的东西
        public:
            struct Holder{
                //包裹器
                ~Holder();
                void *ptr;
                void (*delete_fn)(void *ptr);
                const std::type_info *type;
            };
        public:

            template<class T>
            //普通的构造
            AnyObject(T &&data):AnyObject(
                AnyImpl::DeleteFn<T>,
                std::forward<T>(data)){}
            
            //特殊版本
            AnyObject(int val){
                holder = new Holder{
                    .ptr = new int(val),
                    .delete_fn = AnyImpl::DeleteFn<int>,
                    .type = AnyImpl::TypeInfo<int>()
                };
            }

            template<class T>
            AnyObject(void (*delete_fn)(void*),T &&data){
                holder = new Holder{
                    .ptr = new T(data),
                    .delete_fn = delete_fn,
                    .type = AnyImpl::TypeInfo<T>()
                };
            }
            //初始化一个空的值
            AnyObject():holder(nullptr){};
            ~AnyObject();
            //是否有数值
            bool has_value() const noexcept{
                return holder != nullptr;
            }
            //得到类型
            const std::type_info &type() const;
        public:
            //一些早就有的销毁函数 和类型
        private:
            Holder *holder;
    };
    class BOXAPI AnyTable{
        public:
            //查找
            ~AnyTable();
            const AnyObject &operator[] (const std::string &) const;
            AnyObject &operator[] (const std::string &);
            //有这个键
            bool has_key(const std::string &key) const noexcept;
            //得到里面的实现
            //添加项目
            template<class T>
            void add_item(const std::string &key,T &&data){
                table.emplace(std::make_pair(key,data));
            };
            void add_item(const std::string &key,AnyObject &&);
            std::map<std::string,AnyObject> &get(){
                return table;
            }
        private:
            std::map<std::string,AnyObject> table;
        friend class AnyObject;
    };
    class BOXAPI AnyList{
        public:
            ~AnyList();
        private:
            std::list<AnyObject> objs;
    };
};
#endif
