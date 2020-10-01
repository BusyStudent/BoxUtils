#if !defined(_BOXCOMMON_THREADING_HPP_)
#define _BOXCOMMON_THREADING_HPP_

#include "def.hpp"

#if defined(_WIN32) && !defined(BOX_PTHREAD)
    //如果是Windows上 不用pthead
    #define BOX_WINTHRAD
#elif !defined(BOX_PTHREAD)
    #define BOX_PTHREAD
#endif

//内部实现原理
#ifdef BOX_PTHREAD
    #include <pthread.h>
#endif

#ifdef BOX_WINTHRAD
    #include <windows.h>
#endif

#endif // _BOXCOMMON_THREADING_HPP_

//线程模型的规定
