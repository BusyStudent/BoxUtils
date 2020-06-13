#include <string>
#include <list>
//Linux SIGPIPE信号
#include <thread>
#include <csignal>
#include "threadpool.hpp"
#include "exception.hpp"
#include "dataguard.hpp"
#include "socket.hpp"
#include "sem.hpp"
#include "xv.hpp"
namespace Box{
namespace Xv{
	using namespace Box::Net;
    //服务的实现
    struct ServiceImpl{
        ServiceImpl(unsigned int workers):pool(workers){

        };
        ThreadPool pool;
    };
    void Service::loop(){
        //启动一下
        #ifndef _WIN32
        DataGuard<void(*)(int)> sig_handler_guard(signal(SIGPIPE,SIG_IGN),[](void(*fn)(int)){
            //忽略SIGPIPE
            signal(SIGPIPE,fn);
        });
        #endif
        SockSet sset;
    }
    Service::Service(unsigned int workers){
        service = new ServiceImpl(workers);
    }
    Service::~Service(){
        delete service;
    }
};
};
