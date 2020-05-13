#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include "buffer.hpp"
//重写通知
#define ON_OVER_WRITE(MEM_PTR,SIZE) \
    if(on_overwrite != nullptr){\
    on_overwrite(*this,tail_ptr,byte);\
    }
namespace Box{
    RingBuffer::RingBuffer(size_t size){
        //设置大小
        mem_size = size;
        //申请内存
        mem = (uint8_t*)malloc(size);
        //开始和结束都内存块的开始
        head_ptr = mem;
        tail_ptr = mem;
        //缓冲区结束指针
        mem_tail = mem + mem_size;
        
        on_overwrite = nullptr;
        _private = nullptr;
    }
    RingBuffer::~RingBuffer(){
        free(mem);
    }
    void RingBuffer::extend(size_t new_size){
        //拓展大小
        uint8_t *new_mem = (uint8_t*)realloc(mem,new_size);
        //申请内存
        head_ptr = new_mem + (head_ptr - mem);
        //新的头指针
        tail_ptr = new_mem + (tail_ptr - mem);
        //设置指针和大小
        mem = new_mem;
        mem_size = new_size;
        mem_tail = mem + mem_size;
    }
    //写缓冲区
    void RingBuffer::write(const void *_data,size_t datasize){
        //这里要改一下
        uint8_t *data = (uint8_t*)_data;
        size_t byte;//大小
        while(datasize != 0){
            //复制所有数据进入
            if(head_ptr < tail_ptr){
                //在尾部指针之后
                byte = tail_ptr - head_ptr;//算出空闲的位置
                memcpy(head_ptr,data,byte);
                data += byte;//移动一下
                head_ptr += byte;//移动头
                datasize -= byte;
                //算出尾部和缓冲区末尾的距离
                byte = mem_tail - tail_ptr;
                if(byte > datasize){
                    //剩下空间还够的
                    ON_OVER_WRITE(tail_ptr,datasize);
                    memcpy(tail_ptr,data,datasize);
                    //移动指针
                    tail_ptr += datasize;
                    head_ptr += datasize;
                    return;
                }
                else{
                    //不够 写入byte大小数据
                    datasize -= byte;
                    ON_OVER_WRITE(tail_ptr,byte);
                    memcpy(tail_ptr,data,byte);
                    tail_ptr = mem;//移动到开头
                    head_ptr = mem;
                    data += byte;
                }
            }
            if(head_ptr <= mem_tail){
                //如果开头在内存块的末尾前
                byte = mem_tail - head_ptr;
                if(byte >= datasize){
                    //可用字节大于
                    memcpy(head_ptr,data,datasize);
                    head_ptr += datasize;
                    return;
                }
                else if(byte != 0){
                    //不够了
                    memcpy(head_ptr,data,byte);
                    head_ptr = mem;//移动到开头
                    datasize -= byte;
                    data += byte;
                }
            }
        }
    }
    size_t RingBuffer::read(void *_data,size_t size){
        size_t byte;
        uint8_t *data = (uint8_t*)_data;
        if(tail_ptr == head_ptr){
            //没数据可读入
            return 0;
        }
        else if(tail_ptr > head_ptr){
            //头在尾部后面
            byte = mem_tail - tail_ptr;//可读入数据大小
            if(byte > size){
                //超过了数据块大小
                memcpy(data,tail_ptr,size);
                tail_ptr += size;//移动尾部
                return size;
            }
            //复制完可用所有的部分
            memcpy(data,tail_ptr,byte);
            size -= byte;
            tail_ptr = mem;//移动到最前面
            data += byte;
            //写完了
            if(size == 0 or tail_ptr == head_ptr){
                return byte;
            }
        }
        //在前面读入
        byte = head_ptr - tail_ptr;
        if(byte > size){
            //超过了数据块大小
            memcpy(data,tail_ptr,size);
            tail_ptr += size;//移动尾部
            return size;
        }
        //复制完可用所有的部分
        memcpy(data,tail_ptr,byte);
        size -= byte;
        tail_ptr = mem;//移动到最前面
        data += byte;
        return byte;
    }
    //清空
    void RingBuffer::clear(){
        tail_ptr = mem;
        head_ptr = mem;
    }
};