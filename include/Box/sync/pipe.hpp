#ifndef _BOX_SYNC_PIPE_HPP_
#define _BOX_SYNC_PIPE_HPP_
//线程同步管道
#include <queue>
#include <mutex>
#include <chrono>
#include "sem.hpp"
namespace Box{
    namespace Sync{
        //单向管道
        template<class T>
        class PipeIterator;

        template<class T>
        class Pipe{
            public:
                Pipe(){}
                Pipe(const Pipe &) = delete;
                ~Pipe(){}
                //是否是空的
                bool empty() const{
                    std::lock_guard<std::mutex> locker(mtx);
                    return data_queue.empty();
                };
                unsigned int size() const{
                    std::lock_guard<std::mutex> locker(mtx);
                    return data_queue.size();
                }
                //写数据
                void write(const T &data){
                    std::lock_guard<std::mutex> locker(mtx);
                    bool empty = data_queue.empty();//是否为空
                    data_queue.push(data);
                    //压入
                    if(empty){
                        //唤醒堵塞在那边的读入端
                        empty_ev.set();
                    }
                };
                void read(T &data){
                    //查队列
                    mtx.lock();
                    while(data_queue.empty()){
                        empty_ev.clear();
                        mtx.unlock();
                        //等待唤醒
                        empty_ev.wait();

                        mtx.lock();
                        //如果这里堵塞 下面又变空 会有BUG
                    }
                    data = data_queue.front();
                    data_queue.pop();
                    
                    mtx.unlock();
                };
                //有超时的读入
                template<class Time>
                bool read(T &data,const Time &time){
                    mtx.lock();
                    while(data_queue.empty()){
                        empty_ev.clear();
                        mtx.unlock();
                        
                        if(not empty_ev.wait(time)){
                            //等待超时
                            return false;
                        }
                        //如果这里堵塞 下面又变空 会有BUG
                        mtx.lock();
                    }
                    data = data_queue.front();
                    data_queue.pop();
                    return true;
                }
                //尝试读取
                bool try_read(T &data){
                    std::lock_guard<std::mutex> locker(mtx);
                    if(data_queue.empty()){
                        return false;
                    }
                    else{
                        data = data_queue.front();
                        data_queue.pop();
                        return true;
                    }
                };
                //得到数据
                T poll(){
                    T val;
                    read(val);
                    return val;
                };
                //操作符
                template<class Data>
                Pipe<T> &operator <<(const Data &data){
                    write(data);
                    return *this;
                };
                //读入迭代器
                typedef PipeIterator<T> iterator;
                typedef PipeIterator<T> Iterator;

                Iterator begin(){
                    return Iterator(this);
                };
                Iterator end(){
                    return Iterator(nullptr);
                };
            private:
                std::queue<T> data_queue;//数据队列
                mutable std::mutex mtx;
                Event empty_ev;
        };
        //迭代器
        template<class T>
        class PipeIterator{
            public:
                PipeIterator(Pipe<T> *p):pipe(p){};
                PipeIterator<T> &operator ++(){
                    pipe->read(value);
                    return *this;
                };
                bool operator !=(const PipeIterator<T> &iter) const noexcept{
                    return iter.pipe != pipe;
                }
                bool operator ==(const PipeIterator<T> &iter) const noexcept{
                    return iter.pipe == pipe;
                }
                T &operator *() noexcept{
                    return value;
                }
                const T &operator *() const noexcept{
                    return value;
                }
            private:
                Pipe<T> *pipe;
                T value;
        };
    } // namespace Sync
    
} // namespace Box

#endif