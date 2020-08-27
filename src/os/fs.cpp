//文件操作
#if defined(_WIN32)
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
#endif // _WIN32
#include "common/def.hpp"
#include "raii/scope.hpp"
#include "os/exception.hpp"
#include "os/handle.hpp"
#include "os/fs.hpp"
namespace Box{
namespace OS{
    Handle Fopen(std::string_view filename,int flags,bool throw_err){
        #ifdef _WIN32
        //使用Flags
        HANDLE handle;
        DWORD os_flags;//设置flags
        DWORD create_flags;//创建flags
        //检查Flags
        if(BOX_HASBIT(flags,Flags::WRONLY) or BOX_HASBIT(flags,Flags::APPEND)){
            //只写
            os_flags = GENERIC_WRITE;
        }
        else if(BOX_HASBIT(flags,Flags::RDONLY)){
            //只读
            os_flags = GENERIC_READ;
        }
        else if(BOX_HASBIT(flags,Flags::RDWR)){
            //可读可写
            os_flags = GENERIC_READ | GENERIC_WRITE;
        }
        //检查创建flags
        if(BOX_HASBIT(flags,Flags::APPEND)){
            //添加内容
            //检查是否存在·
            if(_access(filename.data(),F_OK) != 0){
                //不存在
                create_flags = CREATE_ALWAYS;
            }
            else{
                //存在 打开文件
                create_flags = OPEN_ALWAYS;
            }
        }
        else if(BOX_HASBIT(flags,Flags::WRONLY) or BOX_HASBIT(flags,Flags::RDWR)){
            //创建文件
            create_flags = OPEN_ALWAYS;
        }
        else{
            //打开文件
            create_flags = OPEN_EXISTING;
        }
        handle = CreateFileA(
            filename.data(),
            os_flags,
            0,
            nullptr,
            create_flags,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        if(handle == INVALID_HANDLE_VALUE and throw_err){
            //出错 要抛出异常
            throwError();
        }
        else{
            return handle;
        }
        #else
        int os_flags = 0;
        int fd;
        if(BOX_HASBIT(flags,Flags::WRONLY)){
            os_flags = O_WRONLY;
        }
        else if(BOX_HASBIT(flags,Flags::RDONLY)){
            os_flags = O_RDONLY;
        }
        else if(BOX_HASBIT(flags,Flags::RDWR)){
            os_flags = O_RDWR;
        }
        else if(BOX_HASBIT(flags,Flags::APPEND)){
            os_flags = O_APPEND;
        }
        fd = open(filename.data(),os_flags);
        if(fd < 0 and throw_err){
            throwError();
        }
        return fd;
        #endif
    }
    bool Exists(std::string_view filename){
        #ifdef _WIN32
        return _access(filename.data(),F_OK) == 0;
        #else
        return access(filename.data(),F_OK) == 0;
        #endif
    }
    bool Truncate(std::string_view filename,size_t fsize){
        #ifdef _WIN32

        #else
        return truncate(filename.data(),fsize) == 0;
        #endif
    }
    //设置工作目录
    bool SetWorkDir(std::string_view filename){
        #ifdef _WIN32
        return _chdir(filename.data()) == 0;
        #else
        return chdir(filename.data()) == 0;
        #endif
    }
    //得到工作目录
    std::string GetWorkDir(){
        //return std::string(std::filesystem::current_path());
        char *workdir = getcwd(nullptr,0);
        if(workdir == nullptr){
            throwError();
        }
        Box_defer{
            free(workdir);
        };
        return workdir;
    }
}
}