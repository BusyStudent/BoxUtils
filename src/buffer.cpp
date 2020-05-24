#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include "buffer.hpp"
//申请内存
template <class T>
inline T* Allocate(size_t size){
    return static_cast<T*>(malloc(size));
};
template <class T>
inline T* Reallocate(T* ptr,size_t size){
    return static_cast<T*>(realloc(ptr,size));
};
namespace Box{
    Buffer::Buffer(size_t size){
        mem_size = size;
        used_size = 0;
        mem = Allocate<uint8_t>(size);
    }
    Buffer::Buffer(const Buffer &buffer){
        mem = Allocate<uint8_t>(buffer.mem_size);//复制内存块
        mem_size = buffer.mem_size;//设置大小
        used_size = buffer.used_size;
        memcpy(mem,buffer.mem,used_size);//复制内存
    }
    Buffer::Buffer(Buffer &&buffer){
        mem = buffer.mem;
        mem_size = buffer.mem_size;
        used_size = buffer.used_size;
        //复制数据
        buffer.mem = nullptr;
        buffer.mem_size = 0;
        buffer.used_size = 0;
    }
    Buffer::~Buffer(){
        free(mem);
    }
    void Buffer::extend(size_t new_size){
        mem = Reallocate<uint8_t>(mem,new_size);
        mem_size = new_size;
    }
    size_t Buffer::write(const void *data,size_t size){
        if(full()){
            return 0;
        }
        else if(used_size + size > mem_size){
            //超过边界了一定范围 复制可用的
            size_t copy_byte = mem_size - used_size;
            memcpy(mem + used_size,data,copy_byte);
            used_size += copy_byte;
            return size - copy_byte;//复制的大小
        }
        //足够大
        memcpy(mem + used_size,data,size);
        used_size += size;
        return size;
    }
    uint8_t *Buffer::detach(){
        uint8_t *m = mem;
        mem = nullptr;
        mem_size = 0;
        used_size = 0;
        return m;
    }
} // namespace Box
