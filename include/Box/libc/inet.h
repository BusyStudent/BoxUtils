//网络C的接口 封装两个平台不同的部分
#ifndef _BOX_LIBC_INET_H_
#define _BOX_LIBC_INET_H_
#include <errno.h>
#include "types.h"
#ifdef _WIN32
	#define fdopen _fdopen
	//fdopen的宏
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#include <io.h>
	#include <process.h>
    typedef SOCKET Box_socket_t;
    typedef int Box_socklen_t;
    #define BOX_SOCKET_INVAID(S) ((S)==INVALID_SOCKET)
    
    #if defined(_MSC_VER)
        typedef SSIZE_T ssize_t;
    #endif

#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netinet/in.h> 
	#include <netdb.h>
    typedef int Box_socket_t;
    //地址长度
    typedef socklen_t Box_socklen_t;
    #define BOX_SOCKET_INVAID(S) ((S) <= 0)
#endif

#ifdef __cplusplus
extern "C"{
#endif
//C的部分
//关闭socket
extern int  Box_socket_init();//初始化
extern void Box_socket_quit();//关闭
extern int  Box_closesocket(Box_socket_t sock);//关闭Socket
//Socket错误代码
extern int  Box_socket_errno();
extern int  Box_socket_strerror(int);//格式化错误
//连接
extern int  Box_connect(Box_socket_t sock,const void *addr,Box_socklen_t len);
//绑定
extern int  Box_bind(Box_socket_t sock,const void *addr,Box_socklen_t len);
//接收连接
extern Box_socket_t Box_accept(Box_socket_t sock,void *addr,Box_socklen_t *len);

extern Box_ssize_t Box_send(Box_socket_t sock,const void *buf,size_t bufsize,int flags);
extern Box_ssize_t Box_recv(Box_socket_t sock,void *buf,size_t bufsize,int flags);
#ifdef __cplusplus
}
namespace Box{
    namespace libc{
        //不同平台的定义
        using socket_t =  ::Box_socket_t;
        using socklen_t = ::Box_socklen_t;
        //C函数别名
        inline int closesocket(socket_t sock){
            return Box_closesocket(sock);
        }
        inline int  socket_init(){
            return Box_socket_init();
        }
        inline void socket_quit(){
            return Box_socket_quit();
        }
        //发送数据
        inline ssize_t send(socket_t sock,const void *buf,size_t buflen,int flags) noexcept{
            return Box_send(sock,buf,buflen,flags);
        }
        //接收取出数据
        inline ssize_t recv(socket_t sock,void *buf,size_t buflen,int flags) noexcept{
            return Box_recv(sock,buf,buflen,flags);
        }
        //发送到指定位置
        inline ssize_t sendto(socket_t sock,const void *buf,size_t buflen,
                              int flags,const sockaddr *addr,socklen_t len) noexcept{
            #ifdef _WIN32
            return ::sendto(sock,static_cast<const char*>(buf),buflen,flags,addr,len);
            #else
            return ::sendto(sock,buf,buflen,flags,addr,len);
            #endif
        }
        //接收
        inline ssize_t recvfrom(socket_t sock,void *buf,size_t buflen,
                              int flags,sockaddr *addr,socklen_t *len) noexcept{
            #ifdef _WIN32
            return ::recvfrom(sock,static_cast<char*>(buf),buflen,flags,addr,len);
            #else
            return ::recvfrom(sock,buf,buflen,flags,addr,len);
            #endif
        }
        //抹掉类型的
        inline socket_t accept(socket_t sock,void *addr,socklen_t *len){
            return Box_accept(sock,addr,len);
        }
        inline int bind(socket_t sock,const void *addr,socklen_t len){
            return Box_bind(sock,addr,len);
        }
        inline int connect(socket_t sock,const void *addr,socklen_t len){
            return Box_connect(sock,addr,len);
        }
    };
};
#endif

#endif