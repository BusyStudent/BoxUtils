#ifndef _BOX_SUBPROCESS_HPP_
#define _BOX_SUBPROCESS_HPP_
//子进程 仿照Python
#include <cstdio>
#include <csignal>
#ifdef _WIN32
	#include <process.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
#endif
#include "string.hpp"
#include "refptr.hpp"
namespace Box{
	class SubProcess{
		public:
			SubProcess();
			SubProcess(const SubProcess &) = delete;//不能拷贝
			~SubProcess();
			//打开一个新的进程
			void run(const std::vector<std::string> &args,
				bool redirect_in = false,bool redirect_out = false,bool redirect_err = false);//运行
				//redirect代表是否重定向流
			//打开一个
			static RefPtr<SubProcess> Popen(const std::vector<std::string> &args,
				bool redirect_in = false,bool redirect_out = false,bool redirect_err = false);
			static std::vector<std::string> Split(const std::string &);//切割字符串到run需要的vector
			static int System(const std::vector<std::string> &args);//运行一下 出错会出异常
			bool poll(int *ret_val);//尝试得到返回值 失败返回false
			int wait();//等待 得到返回值
			int kill(int sig);//发送信号
			int terminate();//向子进程发送SIGTERM
			//得到属性
			inline pid_t pid(){
				return child_pid;
			}
			inline FILE *&stdin(){
				return _stdin;
			}
			inline FILE *&stdout(){
				return _stdout;
			}
			inline FILE *&stderr(){
				return _stderr;
			}
		private:
			//子进程的输入输出
			FILE *_stdin;
			FILE *_stdout;
			FILE *_stderr;
			pid_t child_pid;
	};
};
#endif
