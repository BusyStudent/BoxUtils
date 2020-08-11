#ifndef _BOX_OS_EXCEPTION_HPP_
#define _BOX_OS_EXCEPTION_HPP_
#include <exception>
#include <string>
#ifdef _WIN32
    #include <BaseTsd.h>
    #include <WinDef.h>
    //为了DWORD
#endif
namespace Box{
    namespace OS{
        #ifdef _WIN32
        //系统的错误代码
        typedef DWORD error_t;
        #else
        typedef int error_t;
        #endif
        struct ErrorCode{
            error_t code;
            operator error_t() const noexcept{
                return code;
            }
            std::string to_string() const;
        };
        class Error:public std::exception{
            public:
                Error(error_t code);
                Error(const Error &);
                ~Error();
                const char *what() const noexcept;
                ErrorCode get() const noexcept{
                    return err;
                };
            private:
                ErrorCode err;
                std::string msg;
        };
        ErrorCode GetError();//得到错误
        [[noreturn]] void throwError();
        [[noreturn]] void throwError(error_t code);
    };
};
#endif