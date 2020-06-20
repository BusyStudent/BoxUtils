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
	#include <fcntl.h>
	#include <unistd.h>
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
LIBC_END