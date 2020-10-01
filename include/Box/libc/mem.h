#ifndef _BOX_LIBC_MEM_H_
#define _BOX_LIBC_MEM_H_
//内存 字符串处理
#include <stdlib.h>
#include <stdint.h>
#include "attr.h"
#ifdef __cplusplus
extern "C"{
#endif

//C的函数
typedef void*(*Box_allocator_t)(size_t);
//内存申请函数
//标准C内存申请函数 应为在MSVC上好像是__stdcall
extern BOXAPI void *Box_malloc(size_t n);
extern BOXAPI void  Box_free(void *ptr);

extern BOXAPI void *Box_kmalloc(size_t n);//系统API申请内存
extern BOXAPI void  Box_kfree(void *ptr);//释放内存
extern BOXAPI void *Box_malloc0(size_t n);//申请一块内存 初始化为0
extern BOXAPI void *Box_realloc0(void *ptr,size_t old_n,size_t new_n);//重新申请 把新的地方设置为0
//如果ptr为nullptr old_n为0 那会等效与Box_malloc0 否则返回nullptr
extern BOXAPI void *Box_aligned_alloc(size_t align,size_t size);//内存对齐的申请内存
extern BOXAPI void *Box_memdup(const void *mem,size_t size,Box_allocator_t);//复制内存
extern BOXAPI void *Box_memdupfrom(const void *mem_begin,const void *mem_end,Box_allocator_t);
//查找内存块 查找mem2 在mem1中
extern BOXAPI void *Box_memmem(const void *mem1,size_t n1,const void *mem2,size_t n2);
//随机填充一个内存块
extern BOXAPI void *Box_memrand(void *mem,size_t n);
//字符串函数
extern BOXAPI char *Box_strdup(const char *str,Box_allocator_t);//复制字符串
extern BOXAPI char *Box_strndup(const char *str,size_t n,Box_allocator_t);
//截取两个指针之间字符串
extern BOXAPI char *Box_strdupfrom(const char *str_begin,const char *str_end,Box_allocator_t);
//字符串比较
extern BOXAPI int   Box_strcasecmp(const char *s1,const char *s2);
extern BOXAPI int   Box_strncasecmp(const char *s1,const char *s2,size_t n);
//得到分页大小
extern BOXAPI long Box_getpagesize();
//得到标准malloc请求内存的大小
extern BOXAPI size_t Box_msize(void *ptr);
#ifdef __cplusplus
}
//C++的包装
namespace Box{
    namespace libc{
        typedef Box_allocator_t allocator;
        //查找内存块
        template<class T>
        void free(T *ptr){
            return ::free(static_cast<void*>(ptr));
        };
        inline void *memmem(const void *mem1,size_t n1,const void *mem2,size_t n2){
            return Box_memmem(mem1,n1,mem2,n2);
        }
        inline void *memdup(const void *mem,size_t size,allocator allocate = malloc){
            return Box_memdup(mem,size,allocate);
        };
        //截取两个指针之间的内存
        inline void *memdup(const void *begin,const void *end,allocator allocate = malloc){
            return Box_memdupfrom(begin,end,allocate);
        };
        inline void *memdupfrom(const void *begin,const void *end,allocator allocate = malloc){
            return Box_memdupfrom(begin,end,allocate);
        };
        inline void *malloc0(size_t size){
            //申请内存
            return Box_malloc0(size);
        };
        inline void *realloc0(void *ptr,size_t old_n,size_t new_n){
            return Box_realloc0(ptr,old_n,new_n);
        };
        template<class T>
        T*    malloc0(size_t size){
            //申请内存
            return static_cast<T*>(Box_malloc0(size));
        };
        template<class T>
        T*    malloc(size_t size){
            //C的malloc
            return static_cast<T*>(::malloc(size));
        }
        template<class T>
        T*    realloc(T *ptr,size_t size){
            return static_cast<T*>(::realloc(ptr,size));
        }
        template<class T>
        T*    realloc0(T *ptr,size_t old_n,size_t new_n){
            return static_cast<T*>(::Box_realloc0(ptr,old_n,new_n));
        }
        inline void *kmalloc(size_t size){
            //设置大小
            return Box_kmalloc(size);
        }
        inline void  kfree(void *ptr){
            return Box_kfree(ptr);
        }
        inline void *aligned_alloc(size_t aligned,size_t size){
            return Box_aligned_alloc(aligned,size);
        }
        //字符串复制一份
        inline char *strdup(const char *str,allocator allocate = malloc){
            return Box_strdup(str,allocate);
        };
        //截取前几个
        inline char *strdup(const char *str,size_t n,allocator allocate = malloc){
            return Box_strndup(str,n,allocate);
        };
        //截取两个指针中间的
        inline char *strdup(const char *begin,const char *end,allocator allocate = malloc){
            return Box_strdupfrom(begin,end,allocate);
        };
        inline char *strndup(const char *str,size_t n,allocator allocate = malloc){
            return Box_strndup(str,n,allocate);
        };
        //截取两个指针中间的
        inline char *strdupfrom(const char *begin,const char *end,allocator allocate = malloc){
            return Box_strdupfrom(begin,end,allocate);
        };
        //字符串比较
        //不区分大小写
        inline int  strcasecmp(const char *s1,const char *s2){
            return Box_strcasecmp(s1,s2);
        };
        inline int  strcasecmp(const char *s1,const char *s2,size_t n){
            return Box_strncasecmp(s1,s2,n);
        };
        inline int  strncasecmp(const char *s1,const char *s2,size_t n){
            return Box_strncasecmp(s1,s2,n);
        };
        //得到分页大小
        inline long getpagesize(){
            return Box_getpagesize();
        };
    };
};
#endif
#endif