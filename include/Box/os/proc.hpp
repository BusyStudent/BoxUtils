#if !defined(_BOX_OS_PROC_HPP_)
#define _BOX_OS_PROC_HPP_
//进程相关
#ifdef _WIN32
    #include <MinWindef.h>
#else
    #include <sys/types.h>
#endif
#include <string_view>
#include "../libc/attr.h"
namespace Box{
    namespace OS{
        class Handle;
        #ifdef _WIN32
        typedef ProcessID ProcID;
        #else
        typedef pid_t ProcID;
        #endif
        struct BOXAPI Proc{
            ProcID pid;//进程ID
            bool terminate();//终止
            int  wait();//等待结束
            bool is_alive() const;//是否存活
            //转换
            operator ProcID() const noexcept{
                return pid;
            }
        };
         //切换为守护进程
        BOXAPI bool Daemon() noexcept;
        //打开进程
        BOXAPI Proc Popen(
            std::string_view cmd,
            Handle *in = nullptr,
            Handle *out = nullptr,
            Handle *err = nullptr
        );
        //生成管道
        BOXAPI void Pipe(Handle &rx,Handle &tx);
    };
};

#endif // _BOX_OS_PROC_HPP_
