#ifndef _BOX_LINUX_HPP_
#define _BOX_LINUX_HPP_
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <string>
namespace Box{
	namespace Linux{
		enum Flag{
			//标识符号
			CREAT = O_CREAT,
			RDONLY = O_RDONLY,//只读
			WRONLY = O_WRONLY,//只写
			RDWR = O_RDWR,//可读可写
			NONBLOCK = O_NONBLOCK,//不堵塞
			SYNC = O_SYNC,//同步的
		};
		struct FD{
			//析构造函数实现
			~FD();
			int fd = -1;//文件描述符
			//清理函数指针
			void (*_cleanup_func)(FD &) = nullptr;
			//一些方法
			bool open(const char *filename,int flags = Flag::CREAT );//打开默认是创建新文件
			bool close();//关闭
			ssize_t write(const void *buf,size_t bufsize);//写
			ssize_t read(void *buf,size_t bufsize);//读入
			//写入字符串
			ssize_t operator <<(const std::string &);
			ssize_t operator <<(int n);
			ssize_t operator <<(double n);
		};
		//把FD定义为Handle
		typedef FD Handle;
		const char *GetError();
		int GetErrorCode();
	};
};
#endif
