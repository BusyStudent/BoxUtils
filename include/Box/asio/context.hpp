#if !defined(_BOX_ASIO_CONTEXT_HPP_)
#define _BOX_ASIO_CONTEXT_HPP_
#include <condition_variable>
#include <chrono>

#include "../libc/atexit.h"
namespace Box{
    namespace Asio{
        enum class RetStatus{
            TiMEOUT,
            NO_TIMEOUT
        };
        class ContextImpl;
        class BOXAPI AsyncResult{
            //等待超时
            public:
                RetStatus wait();
                RetStatus wait(std::chrono::nanoseconds t);
            private:
                std::condition_variable condvar;
        };
        class BOXAPI Context{
            public:
                Context();
                Context(const Context &) = delete;
                Context(Context &&);
                ~Context();
            private:
                ContextImpl *impl;
        };
    };
};


#endif // _BOX_ASIO_CONTEXT_HPP_
