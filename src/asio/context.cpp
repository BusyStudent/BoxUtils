#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #include <sys/epoll.h>
#endif
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <condition_variable>
#include "common/def.hpp"
#include "asio/context.hpp"
#include "libc/atexit.h"
#include "threadpool.hpp"

#ifndef _WIN32
namespace Box{
namespace Asio{
    void thread_wrapper(void *);
    struct WorkThread{
        WorkThread():
            th(StartThread(thread_wrapper,this)){
            //start thread
            
        }
        ~WorkThread(){
            th.join();
        }
        std::thread th;
        std::map<int,epoll_event*> fds_map;//Fd映射的东西
        std::vector<epoll_event> events;
        int epfd;//Epoll fd
        int msgqueue[2];//Message Queue
        void run();
        static void Init();
        static void PushEvent(Event &ev);//压入事件
    };
    struct Event{
        enum {
            READ,
            WRITE
        }types;
        int fd;
        AsyncResult *result;
    };
    //Fd的回调
    struct FdCallback{
        int fd;
        void *userdata;
        void(*do_read)(FdCallback &self);
        void(*do_write)(FdCallback &self);
        void(*do_error)(FdCallback &self);//在发生错误时候调用
        void(*do_drop)(FdCallback &);//在销毁时候调用
        ~FdCallback(){
            if(do_drop != nullptr){
                do_drop(*this);
            }
        }
    };
    WorkThread *work_th = nullptr;
    void thread_wrapper(void *work){
        WorkThread &self = *static_cast<WorkThread*>(work);
        self.epfd = epoll_create(10);
        if(self.epfd < 0 or pipe2(self.msgqueue,O_NONBLOCK) == -1){
            //Error
            return;
        }
        //添加事件callback
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLERR;
        ev.data.fd = self.msgqueue[0];

        epoll_ctl(self.epfd,EPOLL_CTL_ADD,ev.data.fd,&ev);
        self.events.push_back(ev);
        self.run();
    }
    void WorkThread::run(){
        int nfds = epoll_wait(epfd,events.data(),events.size(),-1);
        while(nfds != -1){
            nfds = epoll_wait(epfd,events.data(),events.size(),-1);
        }
    }
    void WorkThread::Init(){
        //Init Work thread
        static std::once_flag flag;
        std::call_once(flag,[](){
            work_th = new WorkThread();
            libc::atexit_once([](){
                delete work_th;
            });
        });
    }
    //压入事件
    void WorkThread::PushEvent(Event &ev){
        static std::mutex mtx;
        std::lock_guard<std::mutex> locker(mtx);
        write(work_th->msgqueue[1],&ev,sizeof(Event));
    }
};
};
#endif