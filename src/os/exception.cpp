#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#include <cerrno>
#include <cstring>
#include "common/def.hpp"
#include "fmt.hpp"
#include "libc/attr.h"
#include "os/exception.hpp"
namespace Box{
namespace OS{
    //错误代码格式化
    std::string ErrorCode::to_string() const{
        #ifdef _WIN32
        char *os_msg;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&os_msg,
			0,
			nullptr);
        std::string msg(msg);
        LocalFree(os_msg);
        return msg;
        #else
        return strerror(code);
        #endif
    }
    Error::Error(error_t code):
        err({code}),
        msg(Format("[OSError {}] {}",code,err.to_string())){
        //初始化错误
    }
    Error::Error(const Error &e):err(e.err),msg(e.msg){

    }
    Error::~Error(){};
    //输出错误信息
    const char *Error::what() const noexcept{
        return msg.c_str();
    }
    //抛出异常
    [[noreturn]] void throwError(error_t code){
        throw Error(code);
    }
    [[noreturn]] void throwError(){
        #ifdef _WIN32
        throw Error(GetLastError());
        #else
        throw Error(errno);
        #endif
    }
    //得到OS错误信息
    ErrorCode GetError(){
        #ifdef _WIN32
        return {
            GetLastError()
        };
        #else
        return {
            errno
        };
        #endif
    }
};
};