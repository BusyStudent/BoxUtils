#ifndef _BOX_LIBC_IO_H_
#define _BOX_LIBC_IO_H_
#ifdef _WIN32
    #include <winbase.h>
    #include <io.h>
    #include <process.h>
#else
    #include <fcntl.h>
#endif
#include "types.h"
#ifdef __cplusplus
extern "C"{
#endif
//打开一个文件
//打开的flags
//打开方式
enum{
    #ifdef _WIN32
    BOX_EMPTY = 0
    #else
    //Linux
    BOX_O_CREATE = O_CREAT,//创建
    BOX_O_APPEND = O_APPEND,//添加在文件末尾
    BOX_O_SYNC   = O_ASYNC,//同步
    BOX_O_WRONLY = O_WRONLY,//写
    BOX_O_RDONLY = O_RDONLY,//读入
    BOX_O_RDWD   = O_RDWR,//读入和写
    #endif
}Box_flags;
extern int Box_fd_errno();//得到错误代码
extern const char *Box_fd_strerror(int);//格式化错误代码
extern Box_fd_t Box_open(const char *path,int flags);//打开
extern int Box_close(Box_fd_t);//关闭

#ifdef __cplusplus
namespace Box{
    namespace libc{
        using fd_t = Box_fd_t;
    };
};
#endif
#endif