#define _BOX_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include "libc/atexit.h"
#include "libc/mem.h"
#include "libc.h"
#ifdef __linux
    #include <sys/random.h>
#endif
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/mman.h>
#endif
LIBC_BEGIN
//内存
#ifndef _WIN32
typedef struct MapedMemory{
    size_t size;
    char memory[];
}MapedMemory;
#endif

BOXAPI void *Box_malloc(size_t n){
    return malloc(n);
}
BOXAPI void  Box_free(void *ptr){
    free(ptr);
}

BOXAPI void *Box_kmalloc(size_t n){
    #ifdef _WIN32
    //没实现
    return VirtualAlloc(nullptr,n,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    #else
    size_t pagesize = getpagesize();
    size_t offset;
    n += sizeof(struct MapedMemory);
    offset = n % pagesize;
    if(offset != 0){
        //没有申请够一个页
        n = n - offset + pagesize;
    }
    MapedMemory *mem = (MapedMemory*)mmap(nullptr,n,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
    if(mem == MAP_FAILED){
        return nullptr;
    }
    else{
        //设置大小
        mem->size = n;
        return mem->memory;
    }
    #endif
}
BOXAPI void Box_kfree(void *ptr){
    #ifdef _WIN32
    if(ptr == nullptr){
        return;
    }
    else{
        if(not VirtualFree(ptr,0,MEM_RELEASE)){
            //失败
            abort();
        }
    }
    #else
    if(ptr == nullptr){
        return;
    }
    MapedMemory *mem = (MapedMemory*)(ptr - offsetof(MapedMemory,memory));
    //指针差
    //检查
    LIBC_ASSERT_PERROR(munmap(mem,mem->size) == 0);
    #endif
}
BOXAPI void *Box_malloc0(size_t n){
    //申请一块内存被设置为0内存
    if(n == 0){
        n = 1;
    }
    void *mem = malloc(n);
    memset(mem,0,n);
    return mem;
}
BOXAPI void *Box_aligned_alloc(size_t aligned,size_t size){
    size_t n = size & aligned;
    //补全
    return malloc(size + aligned - n);
}
BOXAPI void *Box_realloc0(void *ptr,size_t old_n,size_t new_n){
    if(ptr == nullptr){
        if(old_n == 0){
            return Box_malloc0(new_n);
        }
        return nullptr;
    }
    void *mem = realloc(ptr,new_n);
    if(mem == nullptr){
        //失败
        return nullptr;
    }
    if(new_n <= old_n){
        //缩小了
        return mem;
    }
    memset(mem + old_n,0,new_n - old_n);
    return mem;
}
BOXAPI void *Box_memdup(const void *mem,size_t size,Box_allocator_t allocate){
    void *new_mem = allocate(size);
    memcpy(new_mem,mem,size);
    return new_mem;
}
BOXAPI void *Box_memdupform(const void *mem_begin,const void *mem_end,Box_allocator_t allocate){
    return Box_memdup(mem_begin,mem_end - mem_begin,allocate);
}
//查找内存块
//比较函数
static inline int _memncmp(const void *mem1,size_t n1,const void *mem2,size_t n2){
    //比较最小的那个块
    return memcmp(mem1,mem2,n1 > n2 ? n2 : n1);
}
BOXAPI void *Box_memmem(const void *mem1,size_t n1,const void *mem2,size_t n2){
    //检查一下参数
    if(mem1 == nullptr or mem2 == nullptr or n1 == 0 or n2 == 0){
        return nullptr;
    }
    void *pos;
    while(n1 != 0){
        pos = memchr(mem1,*((unsigned char*)mem2),n1);
        //查找第一个字节
        if(pos == nullptr){
            //没找到
            return nullptr;
        }
        else if(_memncmp(pos,mem1 + n1 - pos,mem2,n2) == 0){
            //比较
            return pos;
        }
        else{
            //缩小1
            n1 -= pos + 1 - mem1;
            mem1 = pos + 1;
        }
    }
    return nullptr;
}
//字符串
BOXAPI char *Box_strdup(const char *str,Box_allocator_t allocate){
    size_t len = strlen(str) + 1;
    return (char*) Box_memdup(str,len,allocate);
}
BOXAPI char *Box_strndup(const char *str,size_t n,Box_allocator_t allocate){
    size_t len = strlen(str);
    if(len < n){
        return Box_memdup(str,len + 1,allocate);
    }
    else{
        //把末尾设置为0
        char *mem = (char*) allocate(n + 1);
        memcpy(mem,str,n);
        mem[n] = '\0';
        return mem;
    }
}
//截取两个指针之间字符串
BOXAPI char *Box_strdupfrom(const char *str_begin,const char *str_end,Box_allocator_t allocate){
    if(str_begin == nullptr or str_begin >= str_end){
        return nullptr;
    }
    else if(str_end == nullptr){
        //没有找到末尾
        return Box_strdup(str_begin,allocate);
    }
    size_t size = str_end - str_begin;
    char *mem = (char*) allocate(size + 1);
    memcpy(mem,str_begin,size);
    mem[size] = '\0';
    return mem;
}
//比较 忽略大小
BOXAPI int  Box_strcasecmp(const char *s1,const char *s2){
    //不能有nullptr
    if(s1 == nullptr or s2 == nullptr){
        return 1;
    }
    if(s1 == s2){
        return 0;
    }
    while(*s1 and *s2){
        if(*s1 == *s2 or tolower(*s1) == tolower(*s2)){
            //如果相等
            s1 ++;
            s2 ++;
        }
        else{
            return 1;
        }
    }
    return 0;
};
BOXAPI int  Box_strncasecmp(const char *s1, const char *s2, size_t n){
    if(s1 == nullptr or s2 == nullptr){
        return 1;
    }
    if(s1 == s2){
        return 0;
    }
    size_t rn = 0;//已经读入过的字节
    while(*s1 and *s2 and rn < n){
        if(*s1 == *s2 or tolower(*s1) == tolower(*s2)){
            s1 ++;
            s2 ++;
            rn ++;
        }
        else{
            return 1;
        }
    }
    return 0;
}
//得到分页大小
BOXAPI long Box_getpagesize(){
    #ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
    #else
    return sysconf(_SC_PAGE_SIZE);
    #endif
}
//随机填充内存
BOXAPI void *Box_memrand(void *mem,size_t n){
    #ifdef __linux
    if(getrandom(mem,n,GRND_NONBLOCK) > 0){
        return mem;
    }
    #endif
    //使用seed随机生成数据
    unsigned int seed = time(nullptr);
    for(size_t i = 0; i < n; i++){
        seed = seed *1103515245 +12345;
        ((uint8_t*)mem)[i] =  (uint8_t)((seed/65536) % 32768);
    }
    return mem;
};
LIBC_END