#define _BOX_SOURCE
#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#include <io.h>
	#include <process.h>
    #include <alloca.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netinet/in.h> 
	#include <netdb.h>
#endif
#include "libc/inet.h"
#include "libc.h"
LIBC_BEGIN
BOXAPI int Box_socket_init(){
    //Win32 初始化socket
    #ifdef _WIN32
    WORD version = MAKEWORD(2, 2);
    WSADATA data;
    return WSAStartup(version,&data);
    #else
    return 0;
    #endif
}
BOXAPI void Box_socket_quit(){
    //Win32 清理一下
    #ifdef _WIN32
    WSACleanup();
    #endif
    
}
//关闭socket
BOXAPI int Box_closesocket(Box_socket_t sock){
    #ifdef _WIN32
    return closesocket(sock);
    #else
    return close(sock);
    #endif
}
//发送数据
BOXAPI Box_ssize_t Box_send(Box_socket_t sock,const void *buf,size_t bufsize,int flags){
    #ifdef _WIN32
    return send(sock,(const char*)buf,bufsize,flags);
    #else
    return send(sock,buf,bufsize,flags);
    #endif
}
BOXAPI Box_ssize_t Box_recv(Box_socket_t sock,void *buf,size_t bufsize,int flags){
    #ifdef _WIN32
    return recv(sock,(char*)buf,bufsize,flags);
    #else
    return recv(sock,buf,bufsize,flags);
    #endif
}
//得到错误代码
BOXAPI int Box_socket_errno(){
    #ifdef _WIN32
    return WSAGetLastError();
    #else
    return errno;
    #endif
}
//连接
BOXAPI int Box_connect(Box_socket_t sock,const void *addr,Box_socklen_t len){
    return connect(sock,(const struct sockaddr*)addr,len);
}
BOXAPI int Box_bind(Box_socket_t sock,const void *addr,Box_socklen_t len){
    return bind(sock,(const struct sockaddr*)addr,len);
}
BOXAPI Box_socket_t Box_accept(Box_socket_t sock,void *addr,Box_socklen_t *len){
    return accept(sock,(struct sockaddr*)addr,len);
}
//IP转地址
BOXAPI int Box_inet_pton(int type,const char *str,void *addr){
    #ifdef _WIN32
    //win32
    int len;
    switch(type){
        //类型
        case AF_INET:
            len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            len = sizeof(struct sockaddr_in6);
            break;
        default:
            //暂时不被支持
            WSASetLastError(WSAEPROTONOSUPPORT);
            return -1;
    }
    //这个函数的参数没有const不知道会不会改变参数
    size_t slen = strlen(str);//临时字符串
    char *tstr = alloca((slen + 1) * sizeof(char))
    memcpy(tstr,str,slen * sizeof(char));
    tstr[slen] = '\0';
    return WSAStringToAddressA(tstr,type,nullptr,addr,&len);

    #else
    return inet_pton(type,str,addr);
    #endif
}
//地址转IP
BOXAPI const char *Box_inet_ntop(int type,const void *addr,char *buf,size_t buflen){
    #ifdef _WIN32
    int len;
    switch(type){
        case AF_INET:
            len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            len = sizeof(struct sockaddr_in6);
            break;
        default:
            WSASetLastError(WSAEPROTONOSUPPORT);
            return -1;
    }
    //这个函数的参数没有const不知道会不会改变参数
    void *taddr = alloca(len);//临时的地址
    memcpy(taddr,addr,len);
    if(WSAAddressToStringA(taddr,len,nullptr,buf,buflen) != 0){
        return nullptr;
    }
    else{
        return buf;
    }
    
    #else
    return  inet_ntop(type,addr,buf,buflen);
    #endif
}
LIBC_END