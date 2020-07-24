#ifndef _BOX_SDL_EXCEPTION_HPP_
#define _BOX_SDL_EXCEPTION_HPP_
#include <exception>
namespace Box{
    namespace SDL{
        class Error{
            public:
                Error(const char *msg);
                ~Error();
            private:
                const char *msg;
        };
        [[noreturn]] void throwError(const char *msg);
        [[noreturn]] void throwError();
    };
}
#endif