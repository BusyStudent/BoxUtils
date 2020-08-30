#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2ipdef.h>
#else
    #include <sys/socket.h>
    #include <unistd.h>
    #include <poll.h>
#endif
#include <algorithm>
#include <vector>
#include <cerrno>
#include "common/def.hpp"
#include "net/socket.hpp"
#include "net/poll.hpp"
namespace Box{
namespace Net{
    Pollfds::Pollfds(){}
    Pollfds::Pollfds(int size):pfds(size){}
    Pollfds::Pollfds(Pollfds &&p):pfds(p.pfds){}
    Pollfds::~Pollfds(){}
    //添加
    void Pollfds::add(const Pollfd &pfd){
        pfds.push_back(pfd);
    }
    void Pollfds::add(NativeSocket sock,short event){
        pfds.push_back({
            sock,
            event,
            0
        });
    }
    void Pollfds::add(SocketRef sock,short event){
        pfds.push_back({
            sock.get_fd(),
            event,
            0
        });
    }
    //移除数据
    bool Pollfds::remove(NativeSocket sock){
        iterator iter = begin();
        while(iter != end()){
            if(iter->fd == sock){
                erase(iter);
                return true;
            }
            ++iter;
        }
        return false;
    }
    bool Pollfds::remove(SocketRef sock){
        return Pollfds::remove(sock.fd);
    }
    //进行poll
    int Pollfds::poll(int timeout) noexcept{
        return BOX_POLL(pfds.data(),pfds.size(),timeout);
    }
    #ifdef BOX_NET_EPOLL
    //Epoll的部分
    Epollfds::Epollfds(Epollfds &&e):
        events(e.events),
        nfds(e.nfds),
        epfd(e.epfd){
        //清空原来的epfd
        e.epfd = -1;
    }
    Epollfds::Epollfds(){
        //创建一个epoll节点
        epfd = epoll_create1(0);
        if(epfd < 0){
            //出错了
            throwSocketError();
        }
        nfds = 0;
    }
    Epollfds::~Epollfds(){
        if(epfd > 0){
            close(epfd);
        }
    }
    int Epollfds::wait(int timeout) noexcept{
        //进行等候
        return epoll_wait(epfd,events.data(),nfds,timeout);
    }
    int Epollfds::poll(int timeout) noexcept{
        return epoll_wait(epfd,events.data(),nfds,timeout);
    }
    bool Epollfds::add(NativeSocket fd,EventsType events,void *userdata){
        //创建事件
        EpollEvent ev;
        ev.events = events;
        ev.data.fd = fd;
        ev.data.ptr = userdata;
        if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev) == -1){
            //失败
            //这个我是不是应该返回true?
            //if(errno == EEXIST){
            //    return true;
            //}
            return false;
        }
        else{
            //扩大vector
            this->events.reserve(nfds + 1);
            nfds ++;
            return true;
        }
    }
    bool Epollfds::add(SocketRef sock,EventsType events,void *data){
        return Epollfds::add(sock.fd,events,data);
    }
    //设置fd
    bool Epollfds::set(int fd,EventsType events,void *data){
        EpollEvent ev;
        ev.events = events;
        ev.data.fd = fd;
        ev.data.ptr = data;
        return epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev) == 0;
    }
    bool Epollfds::set(SocketRef sock,EventsType events,void *data){
        return set(sock.fd,events,data);
    }
    //移除fd
    bool Epollfds::remove(int fd){
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr) == -1){
            //失败
            return false;
        }
        else{
            //缩小vector
            this->events.reserve(nfds - 1);
            nfds --;
            return true;
        }
    }
    bool Epollfds::remove(SocketRef sock){
        return Epollfds::remove(sock.fd);
    }
    //迭代器版本
    Epollfds::iterator Epollfds::erase(iterator iter){
        EpollEvent &ev = *iter;
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,ev.data.fd,nullptr) == -1){
            //失败了
            throwSocketError();
        }
        nfds --;
        return events.erase(iter);
    }
    #endif
};
};