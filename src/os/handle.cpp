#ifdef _WIN32
    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
    #include <process.h>
    #define HANDLE_INVAID INVALID_HANDLE_VALUE
    #define BOX_DUP2 _dup2
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>
    //非法句柄
    #define HANDLE_INVAID -1
    #define BOX_DUP2 dup2
#endif
#include <cstdio>
#include "common/def.hpp"
#include "os/handle.hpp"
#include "os/exception.hpp"
namespace Box{
namespace OS{
    Handle::Handle(Handle && h){
        handle = h.handle;
        h.handle = HANDLE_INVAID;
    }
    Handle::~Handle(){
        Handle::close();
    }
    bool Handle::close() noexcept{
        if(not bad()){
            #ifdef _WIN32
            bool val =  ::CloseHandle(handle) == TRUE;
            #else
            bool val =  ::close(handle) == 0;
            #endif
            handle = HANDLE_INVAID;
            return val;
        }
        else{
            return false;
        }
    }
    bool Handle::bad() const noexcept{
        #ifdef _WIN32
        return handle == HANDLE_INVAID;
        #else
        return handle < 0;
        #endif
    }
    //读写
    ssize_t Handle::write(const void *buf,size_t bufsize) noexcept{
        #ifdef _WIN32
        DWORD out_byte;
        //输出字节
        if(WriteFile(handle,buf,bufsize,&out_byte,nullptr) == FALSE){
            return -1;
        }
        else{
            return out_byte;
        }
        #else
        return ::write(handle,buf,bufsize);
        #endif
    }
    ssize_t Handle::read(void *buf,size_t bufsize) noexcept{
        #ifdef _WIN32
        DWORD in_byte;
        //输出字节
        if(ReadFile(handle,buf,bufsize,&in_byte,nullptr) == FALSE){
            return -1;
        }
        else{
            return in_byte;
        }
        #else
        return ::read(handle,buf,bufsize);
        #endif
    }
    //复制句柄
    Handle Handle::dup(){
        #ifdef _WIN32
        Handle h;
        if(DuplicateHandle(
            GetCurrentProcess(),
            handle,
            GetCurrentProcess(),
            &(h.handle),
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        ) == FALSE){
            //失败
            throwError();
        }
        #else
        Handle h(::dup(handle));
        #endif
        if (h.bad()){
            //失败
            throwError();
        }
        else{
            return h;
        }
    }
    //得到句柄
    NativeHandle Handle::get() const noexcept{
        return handle;
    }
    //分离句柄
    NativeHandle Handle::detach() noexcept{
        NativeHandle h = handle;
        handle = HANDLE_INVAID;
        return h;
    }
    //设置是否可以继承
    void Handle::set_inherit(bool val){
        #ifdef _WIN32
        // DWORD flags;
        // if(GetHandleInformation(handle,&flags) == FALSE){
        //     //失败
        //     throwError();
        // }
        WINBOOL ret;
        if(val){
            //设置为可以继承的
            ret = SetHandleInformation(handle,HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT);
        }
        else{
            ret = SetHandleInformation(handle,HANDLE_FLAG_INHERIT,0);
        }
        if(not ret){
            //失败
            throwError();
        }
        #else
        int flags = fcntl(handle,F_GETFL,0);
        if(flags < 0){
            //failed
            throwError();
        }
        if(val){
            //可以
            if(not BOX_HASBIT(flags,O_CLOEXEC)){
                //没有这个flags
                return;
            }
            flags ^= O_CLOEXEC;
        }
        else{
            if(BOX_HASBIT(flags,O_CLOEXEC)){
                //有这个 直接返回
                return;
            }
            flags |= O_CLOEXEC;
        }
        if(fcntl(handle,F_SETFL,flags) < 0){
            throwError();
        }
        #endif
    }
    //是否可继承
    bool Handle::is_inherit() const{
        #ifdef _WIN32
        DWORD flags;
        if(GetHandleInformation(handle,&flags) == FALSE){
            //失败
            throwError();
        }
        return flags == HANDLE_FLAG_INHERIT;
        #else
        int flags = fcntl(handle,F_GETFL,0);
        if(flags < 0){
            //failed
            throwError();
        }
        return (flags & O_CLOEXEC) != O_CLOEXEC;
        #endif
    }
    size_t Handle::size() const{
        #ifdef _WIN32

        #else
        struct stat buf;
        if(fstat(handle,&buf) != 0){
            //Error
            throwError();
        }
        else{
            //得到大小
            return buf.st_size;
        }
        #endif
    }
    //转换为文件描述符
    int Handle::to_fd(){
        #ifdef _WIN32
        HANDLE hd = Handle::detach();
        int fd = _open_osfhandle(hd,0);
        if(fd == -1){
            //失败
            handle = hd;
            throwError();
        }
        return fd;
        #else
        //Linux
        return Handle::detach();
        #endif
    }
    Handle::operator NativeHandle() const noexcept{
        return handle;
    }
    bool Handle::operator ==(const Handle &h) const noexcept{
        return handle == h.handle;
    }
    bool Handle::operator !=(const Handle &h) const noexcept{
        return handle != h.handle;
    }
    //重新定向文件描述符
    #define SET_FD_TO(FD,TARGET) \
        if(BOX_DUP2(FD,TARGET) < 0){\
            throwError();\
        }
    //重定向句柄 在Linux上就是fd
    #ifdef _WIN32
        //在WINDOWS上真麻烦
        #define BOX_OS_STDIN STD_INPUT_HANDLE
        #define BOX_OS_STDOUT STD_OUTPUT_HANDLE 
        #define BOX_OS_STDERR STD_ERROR_HANDLE
        //得到句柄的宏
        #define GET_HANDLE_FROM(NAME) \
            HANDLE handle = GetStdHandle(NAME);\
            if(handle == nullptr){\
                throwError();\
            }\
            if(DuplicateHandle(\
                GetCurrentProcess(),\
                handle,\
                GetCurrentProcess(),\
                &handle,\
                0,\
                FALSE,\
                DUPLICATE_SAME_ACCESS\
            ) == FALSE){\
                return HANDLE();\
            }\
            return Handle(handle);
        //设置句柄的宏
        #define SET_HANDLE_TO(H,NAME,CRT_NAME) \
            HANDLE hd = H.dup().detach();
            if(SetStdHandle(NAME,H.detach()) == FALSE){\
                throwError();\
            }
            //Crt重新定向
            SET_FD_TO(_open_osfhandle(hd,0),CRT_NAME);
    #else
        #define BOX_OS_STDIN STDIN_FILENO
        #define BOX_OS_STDOUT STDOUT_FILENO
        #define BOX_OS_STDERR STDERR_FILENO

        #define BOX_CRT_STDIN STDIN_FILENO
        #define BOX_CRT_STDOUT STDOUT_FILENO
        #define BOX_CRT_STDERR STDERR_FILENO
        //得到fd
        #define GET_HANDLE_FROM(NAME) \
            int fd = dup(NAME);\
            if(fd < 0){\
                throwError();\
            }\
            return fd;
        //设置fd
        #define SET_HANDLE_TO(H,NAME,CRT_NAME) \
            SET_FD_TO(H.detach(),NAME)
    #endif
    //得到下面三个句柄
    Handle Stdin(){
        GET_HANDLE_FROM(BOX_OS_STDIN);
    }
    Handle Stdout(){
        GET_HANDLE_FROM(BOX_OS_STDOUT);
    }
    Handle Stderr(){
        GET_HANDLE_FROM(BOX_OS_STDERR);
    }
    //设置句柄 在Linux上最后一个参数被忽略
    void SetStdin(Handle &&handle){
        SET_HANDLE_TO(handle,BOX_OS_STDIN,BOX_CRT_STDIN);
    }
    void SetStdout(Handle &&handle){
        SET_HANDLE_TO(handle,BOX_OS_STDOUT,BOX_CRT_STDOUT);
    }
    void SetStderr(Handle &&handle){
        SET_HANDLE_TO(handle,BOX_OS_STDERR,BOX_CRT_STDERR);
    }
};
};