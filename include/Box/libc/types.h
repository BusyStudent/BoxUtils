#ifndef _BOX_LIBC_TYPES_H_
#define _BOX_LIBC_TYPES_H_
//各种操作系统的类型
#ifdef _WIN32
    //Windows
    #include <winsock2.h>
    #include <windows.h>
    typedef SOCKET  Box_socket_t;
    typedef SSIZE_T Box_ssize_t;
    typedef HANDLE  Box_fd_t;
#else
    //Linux
    #include <sys/types.h>
    typedef ssize_t Box_ssize_t;//SSIZE_T
    typedef int     Box_fd_t;//文件描述符号
    typedef int     Box_socket_t;
#endif

#ifdef __cplusplus
namespace Box{
    namespace libc{
        //类型别名
        using ssize_t = Box_ssize_t;
        using fd_t    = Box_fd_t;
        using socket_t = Box_socket_t;
    }; // namespace libc 
};
#endif

#endif