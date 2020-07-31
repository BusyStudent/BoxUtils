#ifdef _WIN32
    #include <windows.h>
    #define HANDLE_INVAID nullptr
#else
    #include <unistd.h>
    #include <fcntl.h>
    //非法句柄
    #define HANDLE_INVAID -1
#endif
#include "os/handle.hpp"
#include "os/exception.hpp"
namespace Box{
namespace OS{
    Handle::Handle(NativeHandle h){
        handle = h;
    }
    Handle::Handle(){
        handle = HANDLE_INVAID;
    }
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
            bool val =  ::CloseFile(handle) == TRUE;
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
        return handle == nullptr;
        #else
        return handle < 0;
        #endif
    }
    //读写
    ssize_t Handle::write(const void *buf,size_t bufsize) noexcept{
        #ifdef _WIN32

        #else
        return ::write(handle,buf,bufsize);
        #endif
    }
    ssize_t Handle::read(void *buf,size_t bufsize) noexcept{
        #ifdef _WIN32

        #else
        return ::read(handle,buf,bufsize);
        #endif
    }
    //复制句柄
    Handle Handle::dup(){
        #ifdef _WIN32

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
    void Handle::set_inheritance(bool val){
        #ifdef _WIN32

        #else
        int flags = fcntl(handle,F_GETFL,0);
        if(flags < 0){
            //failed
            throwError();
        }
        if(val){
            //可以
            flags |= O_CLOEXEC;
        }
        else{
            flags ^= O_CLOEXEC;
        }
        if(fcntl(handle,F_SETFL,flags) < 0){
            throwError();
        }
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
};
};