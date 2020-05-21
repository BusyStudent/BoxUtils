#ifndef _BOX_ASSERT_HPP_
#define _BOX_ASSERT_HPP_
//断言
namespace Box{
    //断言失败后
    void AssertFail(const char *filename,int line,const char *func,const char *exp);
    #ifdef NDEBUG
    //如果不用调试
    inline void Assert(bool){
        
    };
    #else
    //需要调试宏
    inline void Assert(bool val){
        if(!val){
            AssertFail(__FILE__,__LINE__,__FUNCTION__,nullptr);
        }
    };
    #endif
};
#ifdef NDEBUG
//宏实现  可以找到具体的参数
    #define BOX_ASSERT(EXP) (EXP)
#else
    #define BOX_ASSERT(EXP) if(!(EXP)){\
        Box::AssertFail(__FILE__,__LINE__,__FUNCTION__,#EXP);\
    }
#endif

#endif