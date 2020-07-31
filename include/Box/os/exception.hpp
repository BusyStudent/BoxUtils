#ifndef _BOX_OS_EXCEPTION_HPP_
#define _BOX_OS_EXCEPTION_HPP_
#include <exception>
#include <string>
namespace Box{
    namespace OS{
        struct ErrorCode{
            int code;
            operator int() const noexcept{
                return code;
            }
            std::string to_string() const;
        };
        class Error:public std::exception{
            public:
                Error(int code);
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
        [[noreturn]] void throwError(int code = GetError());
    };
};
#endif