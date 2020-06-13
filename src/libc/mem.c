#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libc/mem.h"
#include "libc.h"
LIBC_BEGIN
//内存
void *Box_malloc0(size_t n){
    //申请一块内存被设置为0内存
    if(n == 0){
        n = 1;
    }
    void *mem = malloc(n);
    memset(mem,0,n);
    return mem;
}
void *Box_realloc0(void *ptr,size_t old_n,size_t new_n){
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
void *Box_memdup(const void *mem,size_t size,Box_allocator_t allocate){
    void *new_mem = allocate(size);
    memcpy(new_mem,mem,size);
    return new_mem;
}
void *Box_memdupform(const void *mem_begin,const void *mem_end,Box_allocator_t allocate){
    return Box_memdup(mem_begin,mem_end - mem_begin,allocate);
}
//查找内存块
//比较函数
static inline int _memncmp(const void *mem1,size_t n1,const void *mem2,size_t n2){
    //比较最小的那个块
    return memcmp(mem1,mem2,n1 > n2 ? n2 : n1);
}
void *Box_memmem(const void *mem1,size_t n1,const void *mem2,size_t n2){
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
char *Box_strdup(const char *str,Box_allocator_t allocate){
    size_t len = strlen(str) + 1;
    return (char*) Box_memdup(str,len,allocate);
}
char *Box_strndup(const char *str,size_t n,Box_allocator_t allocate){
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
char *Box_strdupfrom(const char *str_begin,const char *str_end,Box_allocator_t allocate){
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
int  Box_strcasecmp(const char *s1,const char *s2){
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
int  Box_strncasecmp(const char *s1, const char *s2, size_t n){
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
LIBC_END