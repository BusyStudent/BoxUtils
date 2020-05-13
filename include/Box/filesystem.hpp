#ifndef _BOX_FILESYSTEM_HPP_
#define _BOX_FILESYSTEM_HPP_
#include <cstdio>
#include <cstdarg>
#include <string>
#ifdef _WIN32
	#include <direct.h>
	#include <dirent.h>
	#include <sys/stat.h>
#else
	#include <dirent.h>
	#include <sys/stat.h>
#endif
#include "refptr.hpp"
//简单的文件系统操作
namespace Box{
	class File{
		//文件流的封装的操作
		public:
			typedef RefPtr <File> Ptr;
		public:
			File(const File &) = delete;//不能拷贝
			File(File &&);//转移
			~File();//销毁
			//方法
			void close();//关闭流
			//从一个文件指针构建
			//打印
			int print(const char *fmt,...);
			int print(const char *fmt,va_list varg);
			//输入
			int scan(const char *fmt,...);
			int scan(const char *fmtv,va_list varg);
			//写和读入
			size_t read(void *buf,size_t bufsize);
			size_t write(const void *buf,size_t bufsize);
			bool readline(std::string &str);//读入一行
			bool ok();//是否没有问题
			bool eof();//是否到了文件末尾
			void clear();//清除错误标记
			//分离
			FILE *detach();
			const char *get_error();//得到错误
			static Ptr From(FILE *fptr);
			static Ptr Open(const std::string &name,const std::string &mode);//打开文件
			static Ptr Tmpfile();
			inline operator FILE*(){
				//转换为文件指针使用
				return fptr;
			}
			//操作符号写出
			inline File& operator <<(const char *str){
				this->write(str,strlen(str));
				return *this;
			}
			inline File& operator <<(const std::string &s){
				this->write(s.c_str(),s.length() * sizeof(char));
				return *this;
			}
			//操作符号读入
			//模板的读入写入
			template <class T>
			inline size_t write(const T&data){
				return write(&data,sizeof(T));
			}
			template <class T>
			inline size_t read(T&data){
				return read(&data,sizeof(T));
			}
		private:
			inline File(FILE *fptr);
			FILE *fptr;

	};
	namespace FS{
		#ifdef _WIN32
		typedef struct _stat64 NativeStat64;
		#else
		typedef struct  stat64 NativeStat64;
		#endif
		//本地的stat
		struct Stat:public NativeStat64{
			bool is_reg() const;//是正常文件
			bool is_dir() const;//是目录
			bool is_fifo() const;//是管道
		};
		std::string Getcwd();
		size_t GetSize(const char *pathname);//得到文件大小
		
		void GetStat(const char *pathname,Stat &);//得到文件状态
		Stat GetStat(const char *pathname);//得到状态
		bool Exists(const char *pathname);//文件是否存在
		void MkDir(const char *name);//创建文件
		void Chdir(const char *path);//改变目录
		const char *GetError();//得到错误
	};
}
#endif
