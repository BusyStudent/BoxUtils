#ifndef _BOX_TABLE_HPP_
#include <map>
#include <list>
#include <string>
#include <cstring>
#include <typeinfo>
#include <initializer_list>
#include "libc/attr.h"
namespace Box{
    //内部一些常量
    class AnyObject;
    class AnyTable;
    class AnyList;
    class JsonRef;
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
        
        template<>
        BOXAPI const std::type_info* TypeInfo<std::wstring>();

        //销毁函数模板
        template<class T>
        BOXAPI void DeleteFn(void *ptr){
            delete static_cast<typename std::remove_reference<T>::type*>(ptr);
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

        template<>
        BOXAPI void DeleteFn<std::wstring>(void *ptr);
    };
    class BOXAPI AnyObject{
        public:
            //不能复制
            AnyObject();//空的
            AnyObject(const AnyObject &) = delete;
            AnyObject(AnyObject &&);
            AnyObject(const char *);//对于字符串的特殊化
            ~AnyObject();
            //模板
            template<class T>
            AnyObject(T &&data){
                //初始化一个对象
                using std::remove_reference;
                ptr = new typename remove_reference<T>::type (
                    std::forward<T>(data)
                );
                delete_fn = AnyImpl::DeleteFn<typename remove_reference<T>::type>;
                typeinfo = AnyImpl::TypeInfo<typename remove_reference<T>::type>();
            }
            //得到类型
            const std::type_info &type() const;
            //得到类型字符串 demangle过后的
            std::string type_name() const;
            //重置
            void reset();
            //类型是否一样
            template<class T>
            bool is_same() const{
                return *AnyImpl::TypeInfo<T>() == type();
            }
            //转换类型
            template<class T>
            T cast() const{
                using std::remove_reference;
                typedef typename remove_reference<T>::type BaseT;//基本类型
                const BaseT *ptr = static_cast<const BaseT*>(load_ptr(
                    *AnyImpl::TypeInfo<BaseT>()
                ));
                //得到指针
                return *ptr;
            }
            //转换类型
            template<class T>
            T cast(){
                using std::remove_reference;
                typedef typename remove_reference<T>::type BaseT;//基本类型
                BaseT *ptr = static_cast<BaseT*>(load_ptr(
                    *AnyImpl::TypeInfo<BaseT>()
                ));
                //得到指针
                return *ptr;
            }
        public:
            //一些操作符号
            //移动
            AnyObject &operator =(AnyObject &&);
            //字符串版本
            AnyObject &operator =(const char *);
            //赋值
            template<class T>
            AnyObject &operator =(T && data){
                reset();
                //重置 设置东西
                using std::remove_reference;
                ptr = new typename remove_reference<T>::type (
                    std::forward<T>(data)
                );
                delete_fn = AnyImpl::DeleteFn<typename remove_reference<T>::type>;
                typeinfo = AnyImpl::TypeInfo<typename remove_reference<T>::type>();
                return *this;
            }
        public:
            //一些于AnyTable AnyList一样接口 免的cast了
            void insert(const std::string &key,AnyObject &&);//插入键值
            void insert(const std::string &key,AnyObject *);//插入键值
            //列表
            void push_back(AnyObject &&);
            void push_front(AnyObject &&);
            //查找键值
            const AnyObject &operator [](const std::string &key) const;
                  AnyObject &operator [](const std::string &key);
            //查找列表
            const AnyObject &operator [](int index) const;
                  AnyObject &operator [](int index);
            static AnyObject ConvertFrom(JsonRef);//从Json转换
        public:
            //得到原始指针
            void *get_ptr() const{
                return ptr;
            }
            //检查类型 读入指针
            const void *load_ptr(const std::type_info &) const;
                  void *load_ptr(const std::type_info &);
        private:
            void *ptr;//保存内容指针
            void (*delete_fn)(void *);//销毁器
            const std::type_info *typeinfo;//类型
    };
    class BOXAPI AnyTable{
        public:
            //一些构造和其他东西
            AnyTable();
            AnyTable(const AnyTable&) = delete;
            AnyTable(AnyTable &&);
            ~AnyTable();
            void clear();//清空
            bool remove(const std::string &key);//移除键值
            void insert(const std::string &key,AnyObject &&);//插入键值
            void insert(const std::string &key,AnyObject *);//插入键值
            //是否有键值
            bool has_key(const std::string &key) const noexcept;
            //查找键值
            const AnyObject &operator [](const std::string &key) const;
                  AnyObject &operator [](const std::string &key);
            //移动一个表
            AnyTable &operator =(AnyTable &&);
            //弹出一个数值 失败返回nullptr
            AnyObject *pop(const std::string &key) noexcept;
        private:
            std::map<std::string,AnyObject*> table;
    };
    //一个容纳任意类型的表
    class BOXAPI AnyList{
        public:
            typedef std::list<AnyObject*>::iterator ListIter;//基础列表迭代器
            typedef std::list<AnyObject*>::const_iterator ListConstIter;//基础列表迭代器
            //迭代器
            struct iterator;
            struct const_iterator;
        public:
            AnyList();
            AnyList(const AnyList &) = delete;
            AnyList(AnyList &&);
            //从列表初始化
            AnyList(std::initializer_list<AnyObject> &&vlist);
            ~AnyList();
            //STL的操作
            AnyObject *pop(int index) noexcept;//弹出一个数值
            void push_back(AnyObject &&);
            void push_front(AnyObject &&);
            bool remove(int index) noexcept;//删除某个数值
            size_t size() const;//大小
            const AnyObject &operator [](int index) const;
                  AnyObject &operator [](int index);
        public:
            //得到开始和结束
            iterator begin(){
                return list.begin();
            };
            iterator   end(){
                return list.end();
            };
            const_iterator begin() const{
                return list.begin();
            };
            const_iterator   end()const{
                return list.end();
            };
            //移除
            iterator erase(const iterator &iter){
                return list.erase(iter);
            };
            //查找 得到迭代器
            const_iterator index(int index) const;
                  iterator index(int index);
        public:
            //得到列表
            std::list<AnyObject*> &get(){
                return list;
            }
        private:
            std::list<AnyObject*> list;
        public:
            //可读可写迭代器
            struct iterator:public ListIter{
                //读元素
                #if __GLIBCXX__ || defined(__GLIBCPP__)
                //LIBSTDC++
                iterator(){};
                iterator(const ListIter & iter){
                    //复制节点
                    _M_node = iter._M_node;
                }
                //#elif defined(_LIBCPP_VERSION)
                #else
                //直接用memcpy方法
                iterator(){};//啥都没有
                iterator(const ListIter &iter){
                    //复制过来
                    memcpy(this,&iter,sizeof(ListIter));
                };
                #endif
                AnyObject &operator *() noexcept{
                    return *ListIter::operator*();
                }
                AnyObject *operator ->() noexcept{
                    return ListIter::operator *();
                }
                //转换类型
                template<class T>
                T cast(){
                    return operator*().cast<T>();
                }
            };
            struct const_iterator:public ListConstIter{
                #if __GLIBCXX__ || defined(__GLIBCPP__)
                //LIBSTDC++
                const_iterator(){};
                const_iterator(const ListConstIter & iter){
                    //复制节点
                    _M_node = iter._M_node;
                }
                //#elif defined(_LIBCPP_VERSION)
                #else
                //直接用memcpy方法
                iterator(){};//啥都没有
                iterator(const ListConstIter &iter){
                    //复制过来
                    memcpy(this,&iter,sizeof(ListIter));
                };
                #endif
                const AnyObject &operator *() noexcept{
                    return *ListConstIter::operator*();
                }
                const AnyObject *operator ->() noexcept{
                    return ListConstIter::operator *();
                }
                //转换类型
                template<class T>
                T cast(){
                    return operator*().cast<T>();
                }
            };
    };
    
};
#endif
