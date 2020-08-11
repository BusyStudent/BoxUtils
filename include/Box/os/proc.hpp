#if !defined(_BOX_OS_PROC_HPP_)
#define _BOX_OS_PROC_HPP_
//进程相关
#include "../libc/attr.h"
namespace Box{
    namespace OS{
        //切换为守护进程
        class Handle;
        struct BOXAPI Proc{
            bool terminate();//终止
        };
        BOXAPI bool Daemon() noexcept;
        //打开进程
        BOXAPI Proc Popen(
            const char *filename,
            Handle *in = nullptr,
            Handle *out = nullptr,
            Handle *err = nullptr
        );
    };
};

#endif // _BOX_OS_PROC_HPP_
