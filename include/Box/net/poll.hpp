#if !defined(_BOX_NET_POLL_HPP_)
#define _BOX_NET_POLL_HPP_
//进行SocketPoll的东西
#include <vector>
#include <chrono>
#include "../libc/atexit.h"
#include "socket.hpp"
#if defined(__linux) || defined(__ANDROID__)
    //具有EPOLL
    #define BOX_NET_EPOLL
    #include <sys/epoll.h>
#endif
namespace Box{
    namespace Net{
        //集合
        //要进行Poll的fd 
        struct Pollfd:public NativePollfd{
            bool bad() const noexcept{
                //检查是否出错
                return (revents & POLLERR) == POLLERR;
            }
            //可写
            bool could_write() const noexcept{
                return (revents & POLLOUT) == POLLOUT;
            }
            bool could_read() const noexcept{
                return (revents & POLLIN) == POLLIN;
            }
            //得到文件描述符
            SocketRef sockfd() const noexcept{
                return fd;
            }
        };
        class BOXAPI Pollfds{
            public:
                template<class ...Args>
                Pollfds(Args &&...args):
                   pfds(std::forward<Args>(args)...){};
                //从给的参数构建
                Pollfds();
                Pollfds(int size);//预先准备好位置
                Pollfds(const Pollfds &) = delete;
                Pollfds(Pollfds &&);
                ~Pollfds();
                void add(const Pollfd &);
                void add(SocketRef,short event);
                void add(NativeSocket,short event);//添加一个
                //移除一个
                bool remove(SocketRef);
                bool remove(NativeSocket);
                int poll(int timeout = -1) noexcept;//查询 默认无限等待
                template<class T>
                int poll(T &&timeout) noexcept{
                    return poll(std::chrono::milliseconds(timeout).count());
                };
                //迭代器
                typedef std::vector<Pollfd>::iterator iterator;
                typedef std::vector<Pollfd>::const_iterator const_iterator;
                iterator begin() noexcept{
                    return pfds.begin();
                }
                iterator end() noexcept{
                    return pfds.end();
                }
                const_iterator begin() const noexcept{
                    return pfds.begin();
                }
                const_iterator end() const noexcept{
                    return pfds.end();
                }
                //移除erase
                iterator erase(const_iterator iter){
                    return pfds.erase(iter);
                }
                //得到数据
                const std::vector<Pollfd> &vec() const noexcept{
                    return pfds;
                }
                std::vector<Pollfd> &vec() noexcept{
                    return pfds;
                }
                //得到大小
                std::vector<Pollfd>::size_type size() const noexcept{
                    return pfds.size();
                }
            private:
                //数据
                std::vector<Pollfd> pfds;//事件vector
        };
        #ifdef BOX_NET_EPOLL
        struct EpollEvent:public epoll_event{
            //用户数据
            template<class T>
            T *userdata() const noexcept{
                return static_cast<T*>(data.ptr);
            }
            SocketRef sockfd() const noexcept{
                return data.fd;
            }
        };
        /*
            README
            当Epollfds里面没有fds时候
            vector.data() == NULL时候会直接返回
        */
        class BOXAPI Epollfds{
            public:
                //函数
                Epollfds();
                Epollfds(int size);//预先准备好位置
                Epollfds(const Epollfds &) = delete;
                Epollfds(Epollfds &&);
                ~Epollfds();
                //添加(重复会失败)
                typedef decltype(EpollEvent::events) EventsType;//事件类型
                //添加一个Socket 可以加用户数据
                bool add(int fd,EventsType events,void *userdata = nullptr);
                bool add(SocketRef,EventsType events,void *userdata = nullptr);
                //移除
                bool remove(int fd);
                bool remove(SocketRef sock);
                //重新设置
                bool set(int fd,EventsType events,void *userdata = nullptr);
                bool set(SocketRef sock,EventsType events,void *userdata = nullptr);
                //进行poll
                int wait(int timeout = -1) noexcept;
                int poll(int timeout = -1) noexcept;
                //用了chrono
                template<class T>
                int wait(T &&time){
                    return wait(std::chrono::milliseconds(time).count());
                }
                template<class T>
                int poll(T &&time){
                    return poll(std::chrono::milliseconds(time).count());
                }
                //迭代器
                typedef std::vector<EpollEvent>::const_iterator const_iterator;
                typedef std::vector<EpollEvent>::iterator iterator;
                //移除一个
                iterator erase(iterator pos);
                //查找vector
                const EpollEvent &operator [](size_t t) const{
                    return events[t];
                }
                EpollEvent &operator [](size_t t){
                    return events[t];
                }

                const std::vector<EpollEvent> &vec() const{
                    return events;
                }
                std::vector<EpollEvent> &vec(){
                    return events;
                }
                //得到fd
                int fd(){
                    return epfd;
                }
            private:
                std::vector<EpollEvent> events;//事件们
                int nfds;//fd的多少
                int epfd;//epoll的fd
        };
        #endif
        
    };
    #ifndef _WIN32
    namespace OS{
        //EPoll和Poll在POSIX上
        using Net::Pollfd;
        using Net::Pollfds;
        #ifdef BOX_NET_EPOLL
        using Net::Epollfds;
        #endif
    };
    #endif
};
#endif // _BOX_NET_POLL_HPP_
