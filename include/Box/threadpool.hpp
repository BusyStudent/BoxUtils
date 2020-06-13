#ifndef _BOX_THREADPOOL_HPP_
#define _BOX_THREADPOOL_HPP_
//线程池子
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <memory>
#include "sem.hpp"
namespace Box{
    class ThreadPool{
        //线程池子
        public:
            //入口点
            struct StartEntry{
                void (*entry)(void *param);
                void *param;
            };
        public:
            ThreadPool(unsigned int workers);
            ThreadPool(const ThreadPool &) = delete;
            ~ThreadPool();
            unsigned int count_tasks()   const;//统计线程池里的任务(不包括正在处理的)
            unsigned int count_workers() const;
            unsigned int count_freeworkers() const;//统计空闲的工人
            void wait()     const noexcept;//等待所有任务完成
            bool busy()     const noexcept;//是否一个可用的worker都没有
            bool no_tasks() const noexcept;//没有任务了
            void set_workers(unsigned int workers);//设置worker的数量
            //FIX ME 注意在 其他线程调用会有BUG 最好在空闲的时候使用
            void add_task(std::function<void()> *fn);//添加任务
            //原始的添加任务
            void add_task(const StartEntry&);
            //添加一个任务(要得到返回值 如果要得到 请用async或者自己实现)
            template<class T,class ...Args>
            void swpan(const T &fn,Args ...args){
                add_task(
                    new std::function<void()>(std::bind(fn,args...))
                );
            }
            template<class T,class ...Args>
            auto async(const T &fn,Args ...args){
                //包装任务
                auto task = std::make_shared<std::packaged_task<decltype(fn(args ...))()>>(
                    std::bind(fn,args...)
                );
                //添加
                add_task(new std::function<void()>([task](){
                    (*task)();
                }));
                return task;
            }
        private:
            struct Worker;
            void set_workers_lockfree(unsigned int workers);//设置worker的数量 无锁
            void worker_main(Worker &);//工作者的入口
            std::list<Worker*> workers;//工作者的池子
            std::condition_variable cond_var;//条件变量
            mutable std::mutex self_mtx;//锁自己的
            mutable std::mutex mtx;//锁队列
            std::list<StartEntry> tasks;//任务
            //没有任务的事件
            mutable Sync::Event empty_event;
    };
};
#endif