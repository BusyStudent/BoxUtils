#ifndef _BOX_COMMON_DEF_HPP_
#define _BOX_COMMON_DEF_HPP_
//原代码的 必须要包括的头文件
#define _BOX_SOURCE
#include "../libc/attr.h"
//一些东西
//是否有哪一位
#define BOX_HASBIT(INPUT,BIT) ((INPUT & BIT) == BIT)
#endif