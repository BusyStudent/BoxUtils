#if !defined(_BOX_NET_POLL_HPP_)
#define _BOX_NET_POLL_HPP_
//进行SocketPoll的东西
#include <vector>
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
                void add(Socket &,short event);
                void add(NativeSocket,short event);//添加一个
                //移除一个
                bool remove(Socket &);
                bool remove(NativeSocket);
                int poll(int timeout = -1) noexcept;//查询 默认无限等待
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
        };
        class BOXAPI Epollfds{
            public:
                //函数
                Epollfds();
                Epollfds(int size);//预先准备好位置
                Epollfds(const Epollfds &) = delete;
                Epollfds(Epollfds &&);
                ~Epollfds();
            private:
                std::vector<EpollEvent> events;//事件们
                int epfd;//epoll的fd
        };
        #endif
        
    };
};
#endif // _BOX_NET_POLL_HPP_
