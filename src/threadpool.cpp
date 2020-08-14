//#define _BOX_SOURCE
#include <condition_variable>
#include <thread>
#include <future>
#include <vector>
#include <utility>
#include <mutex>
#include <memory>
#include "common/def.hpp"
#include "threadpool.hpp"
#include "atomic.hpp"
#include "sem.hpp"
namespace Box{
    struct ThreadPool::Worker{
        //工作者的定义
        //初始化Worker
        Worker():
            running(true),
            is_free(true),
            detached(false){};
        ~Worker(){
            //running = false;
            if(th.joinable()){
                th.join();
            }
        }
        Sync::Atomic<bool> running;
        Sync::Atomic<bool> is_free;//是否空闲
        bool               detached;//分离的
        std::thread     th;
    };
    ThreadPool::ThreadPool(unsigned int wks){
        //启动线程
        set_workers_lockfree(wks);
    }
    ThreadPool::~ThreadPool(){
        wait();//等待所有工作完成
        std::lock_guard<std::mutex> locker(self_mtx);//锁住自己
        for(auto &worker:workers){
            //设置为不运行false
            worker->running = false;
        }
        cond_var.notify_all();//广播
        for(auto &worker:workers){
            delete worker;
        }
        //删除所有任务
        tasks.clear();
    }
    void ThreadPool::worker_main(Worker &worker){
        StartEntry entry;
        //给cond_var准备的锁
        std::mutex cond_mtx;
        std::unique_lock<std::mutex> locker(cond_mtx);
        while(worker.running){
            //一直执行
            mtx.lock();
            //得到函数
            if(tasks.empty()){
                //空的
                mtx.unlock();
                //设置事件
                empty_event.set();
                cond_var.wait(locker);//等待
                continue;
            }
            else{
                //得到一个函数
                entry = tasks.front();
                tasks.pop_front();
                mtx.unlock();
            }
            worker.is_free = false;
            //调用函数
            entry.entry(entry.param);
            worker.is_free = true;
        }
        if(worker.detached){
            //如果是分离的
            delete &worker;
            //删除自己
        }
    }
    bool ThreadPool::busy() const noexcept{
        std::lock_guard<std::mutex> locker(self_mtx);
        for(auto &worker:workers){
            if(worker->is_free){
                //有空闲的
                return false;
            }
        }
        //没有空闲的`
        return true;
    }
    bool ThreadPool::no_tasks() const noexcept{
        std::lock_guard<std::mutex> locker(mtx);
        return tasks.size() == 0;
    }
    //添加任务
    void ThreadPool::add_task(std::function<void()> *fn){
        if(fn == nullptr){
            return;
        }
        std::lock_guard<std::mutex> locker(mtx);
        bool empty = tasks.empty();//保存是否空的
        //压如一下任务
        tasks.push_back({
            .entry = [](void *fn_ptr){
                typedef std::function<void()> Fn;
                std::unique_ptr<Fn> ptr(static_cast<Fn*>(fn_ptr));
                (*ptr)();
            },
            .param = fn
        });
        if(empty){
            //如果是空的 有线程在等待
            cond_var.notify_one();
            //清空事件
            empty_event.clear();
        }
    }
    //原始的添加任务
    void ThreadPool::add_task(const StartEntry &entry){
        std::lock_guard<std::mutex> locker(mtx);
        bool empty = tasks.empty();//保存是否空的
        //压如一下任务
        tasks.push_back(entry);
        if(empty){
            //如果是空的 有线程在等待
            cond_var.notify_one();
            //清空事件
            empty_event.clear();
        }
    }
    //设置workers的数量
    void ThreadPool::set_workers(unsigned int wks){
        std::lock_guard<std::mutex> locker(self_mtx);
        //锁住池子
        set_workers_lockfree(wks);
    }
    void ThreadPool::set_workers_lockfree(unsigned int wks){
        unsigned int size = workers.size();
        if(wks == size){
            //没改变
            return;
        }
        else if(wks < size){
            //缩小workers
            Worker *worker;
            wks = size - wks;
            //得到需要减少的数量
            for(unsigned int i = 0;i < wks; i ++){
                //得到worker
                worker = workers.front();
                workers.pop_front();
                //分离
                worker->th.detach();
                worker->detached = true;
                worker->running = false;
            }
            //FIX ME
            //这可能会导致分离的进程没有被唤醒 堵塞在那里
            cond_var.notify_all();
            return;
        }
        wks -= size;//算出还需要的workers
        for(unsigned int i = 0;i < wks;i++){
            Worker *worker = new Worker();
            //新建一个worker
            std::thread th([this,worker](){
                worker_main(*worker);
            });
            // 新建一个thread
            worker->th = std::move(th);
            workers.push_back(worker);
        }
    }
    void ThreadPool::wait() const noexcept{
        //等到所有任务完成
        if(no_tasks()){
            return;
        }
        empty_event.wait();
    }
    unsigned int ThreadPool::count_tasks() const{
        std::lock_guard<std::mutex> locker(mtx);
        return tasks.size();
    }
    unsigned int ThreadPool::count_workers() const{
        std::lock_guard<std::mutex> locker(self_mtx);
        return workers.size();
    }
    unsigned int ThreadPool::count_freeworkers() const{
        std::lock_guard<std::mutex> locker(self_mtx);
        unsigned int n = 0;
        for(auto &worker:workers){
            if(worker->is_free){
                n++;
            }
        }
        return n;
    }
    //启动线程
    BOXAPI std::thread StartThread(void(*fn)(void*),void *arg){
        return std::thread(fn,arg);
    };
};
//包装函数
extern "C"{
    void _BoxStartThread(void(*start_fn)(void*),void *data,void *std_thread_ptr){
        if(std_thread_ptr == nullptr){
            Box::StartThread(start_fn,data).detach();
        }
        else{
            std::thread *th = static_cast<std::thread*>(std_thread_ptr);
            *th = Box::StartThread(start_fn,data);
        }
    }
}
