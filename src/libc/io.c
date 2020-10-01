#include <string.h>
#include <errno.h>
#include "libc/types.h"
#include "libc/attr.h"
#include "libc/mem.h"
#include "libc/io.h"
#include "libc.h"
#ifdef _WIN32
    #include <windows.h>
    #define BOX_FD_INVAIL(FD) ((FD) == NULL)
#else
	#include <sys/wait.h>
    #include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
    #include <termios.h>
    //检查是否非法
    #define BOX_FD_INVAIL(FD) ((FD) <= 0)
#endif
LIBC_BEGIN
//得到IO操作失败的错误代码
int Box_fd_errno(){
    #ifdef _WIN32
    return GetLastError();
    #else
    return errno;
    #endif
};
Box_fd_t Box_open(const char *path,int flags){
    //打开文件
    return open(path,flags);
};
//得到终端大小
int Box_ttysize(Box_fd_t fd,int *w,int *h){
    #ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if(not GetConsoleScreenBufferInfo(fd,&info)){
        return -1;
    }
    if(w != nullptr){
        *w = info.dwSize.X;
    }
    if(h != nullptr){
        *h = info.dwSize.Y;
    }
    return 0;
    #else
    struct winsize ws;
    if(ioctl(fd,TIOCGWINSZ,&ws) != 0){
        return -1;//失败
    }
    if(w != nullptr){
        *w = ws.ws_col;
    }
    if(h != nullptr){
        *h = ws.ws_row;
    }
    return 0;
    #endif
};
LIBC_END