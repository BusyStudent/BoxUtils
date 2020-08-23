#if !defined(_BOX_OS_FS_HPP_)
#define _BOX_OS_FS_HPP_
//FS
#include <cstddef>
#include <string>
#include <string_view>
#include "../libc/attr.h"
namespace Box{
    namespace OS{
        struct Flags{
            //文件打开Flags
            static constexpr int RDONLY = 0x0000;//只读
            static constexpr int WRONLY = 0x0001;//只写
            static constexpr int APPEND = 0x0002;//写 在后端加入
            static constexpr int RDWR =   0x0003;//可读可写
            //模式
            //static constexpr int TEXT = 0x1000;//文本
            //static constexpr int BINARY = 0x2000;//2尽职
        };
        class Handle;
        //打开文件 最后一个参数代表出错是否抛出Error
        BOXAPI Handle Fopen(std::string_view filename,int flags,bool throw_err = true);
        //文件是否存在
        BOXAPI bool Exists(std::string_view filename);
        //快速创建文件
        BOXAPI bool Truncate(std::string_view filename,size_t fsize);
        //映射文件
        //设置工作目录
        BOXAPI bool SetWorkDir(std::string_view filename);

        //得到工作目录
        BOXAPI std::string GetWorkDir();
    };
    namespace Fs{
        using OS::Fopen;
        using OS::Exists;
        using OS::Truncate;
        using OS::SetWorkDir;
        using OS::GetWorkDir;
    };
};
#endif // _BOX_OS_FS_HPP_
