#if !defined(_BOX_OS_FS_HPP_)
#define _BOX_OS_FS_HPP_
//FS
#include <cstddef>
#include <string>
#include <string_view>
#include "../libc/attr.h"
namespace Box{
    namespace OS{
        class Handle;
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
        struct MemProt{
            //内存保护
            static constexpr int WRTIE = 1 << 0;//写
            static constexpr int READ  = 1 << 1;//读入
            static constexpr int EXEC  = 1 << 2;//执行
            static constexpr int NONE  = 0;//禁止访问
        };
        class BOXAPI FileMapping{
            //文件映射
            public:
                FileMapping(Handle &f,int prot = MemProt::READ | MemProt::WRTIE );
                FileMapping(Handle &f,int prot,size_t fsize);
                FileMapping(const FileMapping &) = delete;
                ~FileMapping();
                //大小
                size_t size() const noexcept{
                    return mem_size;
                }
                //映射地址
                template<class T = void>
                T *addr() const noexcept{
                    return static_cast<T*>(ptr);
                }
                void sync();//同步
            private:
                size_t mem_size;//映射大小
                void *ptr;//地址
                #ifdef _WIN32
                void *handle;//映射句柄
                #endif
        };

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
        using OS::MemProt;
        using OS::FileMapping;
    };
};
#endif // _BOX_OS_FS_HPP_
