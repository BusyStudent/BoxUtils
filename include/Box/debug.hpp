#ifndef _BOX_DEBUG_HPP_
#define _BOX_DEBUG_HPP_
//调试头文件
#ifdef NDEBUG
    //空的宏
    #define BOX_INIT_DEBUG
    #define BOX_DEBUG(X) 
#else
    #include <exception>
    #include <new>
    #include <csignal>
    #include <cstdio>
    #include <string>
    #include "backtrace.hpp"
namespace BoxDebug{
    class Object{
        //调试对象
        public:
            Object(const Object&) = delete;
            Object(){
                fprintf(stderr,"Program start\n");
                //捕获信号
                signal(SIGABRT,[](int sig){
                    ReceiveSignal(sig,"SIGABRT");
                });
                signal(SIGSEGV,[](int sig){
                    ReceiveSignal(sig,"SIGSEGV");
                });
                signal(SIGINT,[](int sig){
                    ReceiveSignal(sig,"SIGINT");
                });
            };
            ~Object(){
                fprintf(stderr,"Program stop\n");
            }
        private:
            static void ReceiveSignal(int sig,const char *sigstr){
                //输出信号
                signal(sig,SIG_DFL);
                fprintf(stderr,"Program receive siganl %s\n",sigstr);
                fprintf(stderr,"DumpStack():=>\n");
                Box::Backtrace::Show();
                raise(sig);
            };
    };
    extern Object ctl;//控制对象
} // namespace BoxDebug

#define BOX_INIT_DEBUG BoxDebug::Object BoxDebug::ctl;
//声明对象 启动调试
#define BOX_DEBUG(FMT,...) fprintf(stderr,"Debug:"#FMT"\n",##__VA_ARGS__);
#endif
//Debug的函数
namespace Box{
    //调试打印
    #ifdef NDEBUG
    //忽略掉
    template<class String,class ...Args>
    void DebugPrint(const String&,Args...){

    };
    template<class Fn,class ...Args>
    void DebugCall(const Fn&,Args...){

    };
    #else
    template<class ...Args>
    void DebugPrint(const char *str,Args &&...args){
        fputs("\033[32mDEBUG: \033[0m",stderr);
        fprintf(stderr,str,args...);
        fputc('\n',stderr);
    };
    template<class ...Args>
    void DebugPrint(const std::string &str,Args &&...args){
        DebugPrint(str.c_str(),args...);
    };
    template<class ...Args>
    void DebugLog(){

    };
    //在调试的时候调用
    template<class Fn,class ...Args>
    void DebugCall(const Fn &fn,Args &&...args){
        fn(args...);
    };
    #endif
};
#endif