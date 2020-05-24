#ifndef _BOX_BUFFER_HPP_
#define _BOX_BUFFER_HPP_
//缓冲区
#include <cstddef>
#include <cstdint>
namespace Box{
    class Buffer{
        public:
            Buffer(size_t size);
            Buffer(const Buffer &);
            Buffer(Buffer &&);
            ~Buffer();
            size_t write(const void *data,size_t size);//写入缓冲区
            inline size_t size() const noexcept{
                return used_size;//大小
            }
            inline size_t available_size() const noexcept{
                //可用大小
                return mem_size - used_size;
            }
            inline size_t capacity() const noexcept{
                return mem_size;//容量
            }
            inline bool empty() const noexcept{
                //空的
                return used_size == 0;
            }
            inline bool full() const noexcept{
                return mem_size == used_size;
            }
            inline uint8_t *data() const noexcept{
                //得到数据
                return mem;
            }
            inline void move_forward(size_t byte) noexcept{
                //移动缓冲区一读入字节的大小
                used_size += byte;
            }
            inline void clear(){
                //清空数据
                used_size = 0;
            }
            uint8_t *detach();//分离数据
            void extend(size_t new_size);//扩展大小
        protected:
            uint8_t *mem;
            size_t mem_size;//数据大小
            size_t used_size;
    };
};
#endif