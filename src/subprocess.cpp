#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
	#define pipe _pipe
#else
	#include <sys/wait.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include "exception.hpp"
#include "subprocess.hpp"
#define FCLOSE(PTR) \
	if(PTR != nullptr){\
		fclose(PTR);\
	}
using namespace Box;
SubProcess::SubProcess(){
	//清空内部
	child_pid = -1;
	_stdout = nullptr;
	_stderr = nullptr;
	_stdin = nullptr;
}
SubProcess::~SubProcess(){
	//关闭流
	FCLOSE(_stdout);
	FCLOSE(_stderr);
	FCLOSE(_stdin);
}
//尝试重定向
#define REDIRECT(VAL,FDS) \
	if(VAL){\
		assert(pipe(FDS) == 0);\
	}
//运行参数
void SubProcess::run(const String::Vector &args,bool re_in,bool re_out,bool re_err){
	if(args.size() == 0){
		//一个参数也没有
		throw std::invalid_argument("args.size() == 0");
	}
	//fds的0为主进程使用 1 为fork出来使用
	int fds[2];//和子进程通讯的fds 在execvp时候被关闭
	assert(pipe2(fds,O_CLOEXEC) == 0);//创建管道
	int in_fds[2] = {-1,-1};//要被重定向的fd
	int out_fds[2] = {-1,-1};
	int err_fds[2] = {-1,-1};
	//尝试重定向
	REDIRECT(re_in,in_fds);
	REDIRECT(re_out,out_fds);
	REDIRECT(re_err,err_fds);
	child_pid = fork();//进行fork
	if(child_pid < 0){
		//失败
		//....
		//清理
		close(fds[0]);
		close(fds[1]);
		close(in_fds[1]);
		close(in_fds[0]);
		close(out_fds[1]);
		close(out_fds[0]);
		close(err_fds[1]);
		close(err_fds[0]);
		throw OSError(errno);
	}
	else if(child_pid == 0){
		//是fork出来的进程
		char *argv[args.size() + 1] = {nullptr};//生成数组
		//重定向输入输出
		//REDIRECT_FD(re_in,STDIN_FILENO);
		//填充数组
		for(unsigned int i = 0;i < args.size(); i++){
			argv[i] = strdup(args[i].c_str());
		}
		close(fds[0]);//关闭读入端
		if(re_in){
			//重定向输入端口
			close(in_fds[1]);//关闭写端
			dup2(in_fds[0],STDIN_FILENO);
		}
		//重新定向stdout
		if(re_out){
			//关闭读入段
			close(out_fds[0]);
			dup2(out_fds[1],STDOUT_FILENO);
		}
		if(re_err){
			close(err_fds[0]);
			dup2(err_fds[1],STDERR_FILENO);
		}
		execvp(argv[0],argv);
		//失败了
		int err = errno;
		write(fds[1],&err,sizeof(int));//写出errno
		_exit(EXIT_FAILURE);
		//退出
	}
	else{
		//主进程
		close(fds[1]);//关闭写入端
		if(re_in){
			//重新定向输入
			close(in_fds[0]);//关闭读入端口
			_stdin = fdopen(in_fds[1],"w");
		}
		if(re_out){
			close(out_fds[1]);//关闭写
			_stdout = fdopen(out_fds[0],"r");
		}
		if(re_err){
			close(err_fds[1]);//关闭写
			_stderr = fdopen(err_fds[0],"r");
		}
		auto handler = signal(SIGPIPE,SIG_IGN);//忽略信号
		int err;//错误代码
		if(read(fds[0],&err,sizeof(int)) <= 0){
			//读入错误大小失败 代表正常
			close(fds[0]);
			signal(SIGPIPE,handler);//重置回去
			return;
		}
		signal(SIGPIPE,handler);//重置回去
		close(fds[0]);
		FCLOSE(_stdin);
		FCLOSE(_stdout);
		FCLOSE(_stderr);
		_stdin = nullptr;
		_stdout = nullptr;
		_stderr = nullptr;
		//抛出错误
		throw OSError(err,strerror(err),args[0].c_str());
	}
}
//等待
int SubProcess::wait(){
	//堵塞 直到子进程死亡
	int status;
	if(waitpid(child_pid,&status,0) != child_pid){
		//失败
		//....
	}
	return status;
}
//尝试得到返回值
bool SubProcess::poll(int *ret_val){
	if(waitpid(child_pid,ret_val,WNOHANG) == 0){
		//没有结束
		return false;
	}
	return true;
}
//发送信号
static inline int os_kill(pid_t pid,int sig){
	return kill(pid,sig);
}
int SubProcess::kill(int sig){
	return os_kill(child_pid,sig);
}
//发送terminate
int SubProcess::terminate(){
	return this->kill(SIGTERM);
}
RefPtr<SubProcess> SubProcess::Popen(const String::Vector &args,bool re_in,bool re_out,bool re_err){
	RefPtr<SubProcess> proc(new SubProcess());
	proc->run(args,re_in,re_out,re_err);
	return proc;
}

String::Vector SubProcess::Split(const std::string &val){
	return String::Split(val.c_str()," ");
}
//System
int SubProcess::System(const String::Vector &args){
	return Popen(args)->wait();
}