#ifndef _BOX_ATOMIC_HPP_
#define _BOX_ATOMIC_HPP_
#include <mutex>
namespace Box{
    namespace Sync{
        template<class T>
        class Atomic{
            //通过过锁实现的
            public:
                Atomic(T val):value(val){
                    //赋值
                };
                Atomic(const Atomic &) = delete;
                ~Atomic(){};
                T load() const noexcept{
                    //加载变量
                    std::lock_guard<std::mutex> locker(mtx);
                    return value;
                }
                void store(const T &new_value) noexcept{
                    //存储
                    std::lock_guard<std::mutex> locker(mtx);
                    value = new_value;
                }
                //转换变量
                operator T() const noexcept{
                    return load();
                }
                Atomic &operator =(const T &new_value) noexcept{
                    if(&value != &new_value){
                        //避免自我赋值
                        store(new_value);    
                    }
                    return *this;
                }
                Atomic &operator =(const Atomic<T> &val) noexcept{
                    if(this != &val){
                        store(val.load());
                    }
                    return *this;
                }
                /*
                bool operator ==(const Atomic<T> &val) noexcept{
                    return load() == val.load();
                }
                bool operator !=(const Atomic<T> &val) noexcept{
                    return load() != val.load();
                }
                */
            private:
                mutable std::mutex mtx;//锁
                T value;
        };
    };
};
#endif