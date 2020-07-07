#include <Box/threadpool.hpp>
#include <thread>
#include <iostream>
#include <Box/debug.hpp>
BOX_INIT_DEBUG;
int main(){
    Box::ThreadPool pool(3);
    #if 1
    pool.add_task(new std::function<void()>([](){
        std::cout << "What 1 second" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }));
    pool.swpan([](int u){
        std::cout << u << std::endl;
    },1);
    pool.async([](){
        std::cout << "Helo" << std::endl;
    }).wait();
    pool.swpan([&pool](){
        std::cout << "FreeWorkers:"<< pool.count_freeworkers() << std::endl;
        //设置一个worker
        pool.async([](){
            std::cout << "Helo" << std::endl;
        });
    });
    #endif
    //pool.set_workers(1);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    auto future = pool.async([]() -> int{
        return 100;
    });
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << future.get() << std::endl;
}