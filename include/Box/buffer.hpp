#ifndef _BOX_BUFFER_HPP_
#define _BOX_BUFFER_HPP_
//缓冲区
#include <cstddef>
#include <cstdint>
namespace Box{
    class RingBuffer{
        public:
            RingBuffer(size_t size);
            ~RingBuffer();
            void clear();//清空
            void extend(size_t new_size);//扩展大小
            void write(const void *data,size_t datasize);//写入字节
            size_t read(void *data,size_t datasize);//拷贝出来
            //拉去取出 没有可读入的返回false readsize是数据大小 ptr是所给的指针
            bool poll(const void *&ptr,size_t &readsize);
            void *_private;//保留的用户数据 缓冲区内部不会使用这个
        private:
            //在数据被要被覆盖的时候调用 mem是覆盖数据块的开始byte 是大小
            void (*on_overwrite)(RingBuffer &buffer,const void *mem,size_t byte);
            size_t mem_size;//数据块大小
            uint8_t *mem;//整个缓冲区的内存
            uint8_t *mem_tail;//整个缓冲区结束的指针
            uint8_t *head_ptr;//缓冲区数据段开始的头指针
            uint8_t *tail_ptr;//缓冲区数据段结束的指针
    };
};
#endif