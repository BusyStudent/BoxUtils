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
#include "common/def.hpp"
#include "net/poll.hpp"
#include "socket.hpp"
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
    void Pollfds::add(Socket &sock,short event){
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
    bool Pollfds::remove(Socket &sock){
        return Pollfds::remove(sock.get_fd());
    }
    //进行poll
    int Pollfds::poll(int timeout) noexcept{
        return BOX_POLL(pfds.data(),pfds.size(),timeout);
    }
    #ifdef BOX_NET_EPOLL
    //Epoll的部分
    Epollfds::Epollfds(){
        //创建一个epoll节点
        epfd = epoll_create(1);
        if(epfd < 0){
            //出错了
            throwSocketError();
        }
    }
    Epollfds::~Epollfds(){
        close(epfd);
    }
    #endif
};
};