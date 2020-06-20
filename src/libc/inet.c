#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#include <io.h>
	#include <process.h>
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
int Box_socket_init(){
    //Win32 初始化socket
    #ifdef _WIN32
    WORD version = MAKEWORD(2, 2);
    WSADATA data;
    return WSAStartup(version,&data);
    #else
    return 0;
    #endif
}
void Box_socket_quit(){
    //Win32 清理一下
    #ifdef _WIN32
    WSACleanup();
    #endif
}
//关闭socket
int Box_closesocket(Box_socket_t sock){
    #ifdef _WIN32
    return closesocket(sock);
    #else
    return close(sock);
    #endif
}
//发送数据
Box_ssize_t Box_send(Box_socket_t sock,const void *buf,size_t bufsize,int flags){
    #ifdef _WIN32
    return send(sock,(const char*)buf,bufsize,flags);
    #else
    return send(sock,buf,bufsize,flags);
    #endif
}
Box_ssize_t Box_recv(Box_socket_t sock,void *buf,size_t bufsize,int flags){
    #ifdef _WIN32
    return recv(sock,(char*)buf,bufsize,flags);
    #else
    return recv(sock,buf,bufsize,flags);
    #endif
}
//得到错误代码
int Box_socket_errno(){
    #ifdef _WIN32
    return WSAGetLastError();
    #else
    return errno;
    #endif
}
LIBC_END