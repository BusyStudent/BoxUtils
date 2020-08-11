#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include "common/def.hpp"
#include "buffer.hpp"
#include "libc/mem.h"
//申请内存
namespace Box{
    Buffer::Buffer(size_t size){
        mem_size = size;
        used_size = 0;
        mem = libc::malloc<uint8_t>(size);
    }
    Buffer::Buffer(const Buffer &buffer){
        mem = libc::malloc<uint8_t>(buffer.mem_size);//复制内存块
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
        libc::free<uint8_t>(mem);
    }
    void Buffer::extend(size_t size){
        //拓展
        if(size == 0){
            return;
        }
        mem = libc::realloc<uint8_t>(mem,size + mem_size);
        mem_size +=  size;
    }
    void Buffer::resize(size_t new_size){
        //重新设置大小
        mem = libc::realloc<uint8_t>(mem,new_size);
        mem_size = new_size;
        if(used_size > new_size){
            //缩小了
            used_size = new_size;
        }
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
            return copy_byte;//复制的大小
        }
        //足够大
        memcpy(mem + used_size,data,size);
        used_size += size;
        return size;
    }
    void Buffer::append(const void *data,size_t size){
        size_t av_size = available_size();
        if(size > av_size){
            //需要拓展
            extend(size);
        }
        //复制
        memcpy(mem + used_size,data,size);
        used_size += size;
    }
    uint8_t *Buffer::detach(){
        uint8_t *m = mem;
        mem = nullptr;
        mem_size = 0;
        used_size = 0;
        return m;
    }
} // namespace Box
