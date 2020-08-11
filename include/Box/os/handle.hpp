#ifndef _BOX_HANDLE_HPP_
#define _BOX_HANDLE_HPP_
#ifdef _WIN32
    #include <BaseTsd.h>
#else
    #include <sys/types.h>
#endif
#include <cstdlib>
#include "../libc/attr.h"
//基本文件句柄
namespace Box{
    namespace OS{
        #ifdef _WIN32
        typedef void *NativeHandle;
        #else
        typedef int NativeHandle;
        #endif
        class BOXAPI Handle{
            public:
                #ifdef _WIN32
                using ssize_t = SSIZE_T;
                #else
                using ssize_t = ::ssize_t;
                #endif
                explicit Handle();
                Handle(NativeHandle);
                Handle(const Handle &) = delete;
                Handle(Handle &&);
                ~Handle();
                //得到Handle
                NativeHandle get() const noexcept;
                //分离Handle
                NativeHandle detach() noexcept;
                bool close() noexcept;//关闭
                bool bad() const noexcept;//是否无效
                //读和写
                ssize_t write(const void *buf,size_t bufsize) noexcept;
                ssize_t read (void *buf,size_t bufsize) noexcept;
                //拷贝
                Handle dup();
                //设置是否可以继承
                void set_inherit(bool val = true);
                bool  is_inherit() const;//是否可继承
                //操作符号
                
                explicit operator NativeHandle() const noexcept;
                bool operator ==(const Handle &) const noexcept;
                bool operator !=(const Handle &) const noexcept;

                bool operator ==(NativeHandle) const noexcept;
                bool operator !=(NativeHandle) const noexcept;
            private:
                //基本的句柄
                NativeHandle handle;
        };

        BOXAPI Handle Stdin();//得到stdin的句柄
        BOXAPI Handle Stdout();//得到stdout
        BOXAPI Handle Stderr();
        //设置Handle
        BOXAPI void SetStdin (Handle &&);
        BOXAPI void SetStdout(Handle &&);
        BOXAPI void SetStderr(Handle &&);
    };
};
#endif