#ifndef _BOX_STREAM_HPP_
#include <cstddef>
#include <cstdint>
namespace Box{
    //流的抽象定义
    typedef int ErrorCode;//错误代码
    enum class SEEK{
        SET,
        END,//末尾
        CUR//当前位置
    };
    class Stream{
        public:
            //如果错误代码不填入 失败就会抛出异常
            typedef int (Stream::*CloseFn)(ErrorCode *);//关闭文件流 返回0代表成功
            typedef size_t (Stream::*ReadFn)(void*,size_t,ErrorCode *);
            typedef size_t (Stream::*WriteFn)(const void*,size_t,ErrorCode *);
            typedef size_t (Stream::*SeekFn)(size_t,SEEK,ErrorCode *);
        public:
            virtual ~Stream();
            inline int close(ErrorCode * err = nullptr){
                //关闭函数
                return (this->*close_fn)(err);
            }
            inline size_t write(const void *data,size_t size,ErrorCode *err = nullptr){
                //写入
                return (this->*write_fn)(data,size,err);
            }
            inline size_t read(void *data,size_t size,ErrorCode *err = nullptr){
                //读入
                return (this->*read_fn)(data,size,err);
            }
            inline size_t seek(size_t offset,SEEK where,ErrorCode *err = nullptr){
                return (this->*seek_fn)(offset,where,err);
            }
        protected:
            CloseFn close_fn;//关闭流的函数
            WriteFn write_fn;//写
            ReadFn read_fn;//读入
            SeekFn seek_fn;//移动
    };
};
#endif