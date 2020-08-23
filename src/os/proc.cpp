#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <wait.h>
#endif
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include "common/def.hpp"
#include "os/exception.hpp"
#include "os/handle.hpp"
#include "os/proc.hpp"
#include "libc/mem.h"
//SIGNAL SIGPIPE
#include "dataguard.hpp"
#define LINUX_ASSERT(EXP) if(EXP != 0){ Box::OS::throwError();}
namespace Box{
#ifdef _WIN32
namespace Win32{
    //内部WIn32实现
}
#else
namespace Posix{
    struct CmdInfo{
        ~CmdInfo(){
            //释放详情
            for(auto &str:args){
                if(str != nullptr){
                    libc::free(str);
                }
            }
            libc::free(exec_name);
        }
        typedef char *CString;//C字符串
        CString  exec_name;//执行名字
        std::vector<CString> args;
    };
    //检查是否都是Space
    static inline bool CheckAllSpace(const char *str){
        while(*str == ' '){
            ++str;
        }
        //暂停位置是\0
        return *str == '\0';
    };
    //跳过空格 会改变原来的指针
    static inline bool SkipSpace(const char **const pstr){
        while(**pstr == ' '){
            //递增字符串
            ++(*pstr);
        }
        //检查是否是\0
        return **pstr == '\0';
    };
    CmdInfo *ParseCommand(const char *cmd){
        //FIXME 内存BUG
        if(cmd == nullptr){
            return nullptr;
        }
        const char *name_end = strchr(cmd,' ');
        std::vector<CmdInfo::CString> args;//参数
        if(name_end == nullptr){
            //没找到参数
            empty_args:
            char * name  = libc::strdup(cmd);
            args.push_back(libc::strdup(cmd));
            args.push_back(nullptr);
            return new CmdInfo{
                name,
                args
            };
        }
        if(CheckAllSpace(name_end)){
            goto empty_args;
        }
        char *name = libc::strdupfrom(cmd,name_end);//切割出名字
        args.push_back(libc::strdup(name));//压入要执行的名字
        while(not SkipSpace(&name_end)){
            //查到了
            const char *end = strchr(name_end,' ');//查找末尾
            if(end == nullptr or CheckAllSpace(end)){
                //没找到 到达末尾
                //复制当前地args
                args.push_back(libc::strdup(name_end));
                break;
            }
            else{
                args.push_back(libc::strdupfrom(name_end,end));
            }
        }
        //结束的nullptr
        args.push_back(nullptr);
        //调试输出参数
        #ifndef NDEBUG
        fprintf(stderr,"DEBUG INFO in %s\n",__FUNCTION__);
        fprintf(stderr,"  args = %d\n",static_cast<int>(args.size()));
        int i = 0;
        for(auto &str:args){
            fprintf(stderr,"  argv[%d] = %s\n",i,str);
            i++;
        }
        #endif
        return new CmdInfo{
            name,
            args
        };
    };
};
#endif
namespace OS{
    bool Daemon() noexcept{
        #ifdef _WIN32
        return FreeConsole() == TRUE;
        #else
        //不重定向当前工作目录 分离控制台
        return daemon(1,0) == 0;
        #endif
    }
    Proc Popen(std::string_view cmd,Handle *in,Handle *out,Handle *err){
        #ifdef _WIN32
        abort();
        #else
        //解析命令行
        using Posix::CmdInfo;
        using Posix::ParseCommand;
        std::unique_ptr<CmdInfo> info(ParseCommand(cmd.data()));
        if(info == nullptr){
            //解析失败 非法参数
            throwError(EINVAL);
        }
        int err_fds[2];//书写exec错误的fds
        //申请fds
        LINUX_ASSERT(pipe2(err_fds,O_CLOEXEC));
        pid_t child = fork();

        if(child == -1){
            //失败
            throwError();
        }
        if(child == 0){
            //子进程
            //重定向Handle
            close(err_fds[0]);//关闭读出端
            if(in != nullptr){
                //设置可继承
                in->set_inherit();
                if(dup2(in->get(),STDIN_FILENO) != STDIN_FILENO){
                    goto has_err;
                }
            }
            if(out != nullptr){
                out->set_inherit();
                if(dup2(out->get(),STDOUT_FILENO) != STDOUT_FILENO){
                    goto has_err;
                }
            }
            if(err != nullptr){
                err->set_inherit();
                if(dup2(err->get(),STDERR_FILENO) != STDERR_FILENO){
                    goto has_err;
                }
            }
            execvp(info->exec_name,info->args.data());
            //失败了
            has_err:
            int code = errno;
            //写错误代码
            write(err_fds[1],&code,sizeof(int));
            //关闭
            _Exit(EXIT_FAILURE);
        }
        else{
            close(err_fds[1]);//关闭写入端
            typedef decltype(signal(SIGPIPE,SIG_IGN)) SignalHandler;
            //保存当前的signal handler
            DataGuard<SignalHandler> holder(
               signal(SIGPIPE,SIG_IGN),
               [](SignalHandler handler){
                   signal(SIGPIPE,handler);
               }
            );
            int code;
            if(read(err_fds[0],&code,sizeof(int)) == sizeof(int)){
                //读到错误了
                //关闭错误句柄
                close(err_fds[0]);
                //抛出异常
                throwError(code);
            }
            //关闭句柄
            close(err_fds[0]);
            return {child};
        }
        #endif
    }
    //进程操作
    bool Proc::terminate(){
        #ifdef _WIN32

        #else
        return kill(pid,SIGTERM) == 0;
        #endif
    }
    //是否存活
    bool Proc::alive(){
        #ifdef _WIN32

        #else
        return ::kill(pid,0) == 0;
        #endif
    }
    int  Proc::wait(){
        #ifdef _WIN32

        #else
        //等待结束
        int status;
        if(waitpid(pid,&status,0) != pid){
            //等待失败
            throwError();
        }
        return status;
        #endif
    }
    //创建管道
    void Pipe(Handle &rx,Handle &tx){
        #ifdef _WIN32

        #else
        int fds[2];
        if(pipe(fds) != 0){
            //失败
            throwError();
        }
        else{
            rx.close();
            tx.close();
            //给Handle
            rx.handle = fds[0];
            tx.handle = fds[1];
        }
        #endif
    }
};
};