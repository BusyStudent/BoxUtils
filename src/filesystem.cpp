#ifdef _WIN32
	#include <io.h>
	#include <direct.h>
	#include <dirent.h>
	#define access _access
	#define getcwd _getcwd
	#define mkdir(NAME,MODE) _mkdir(NAME)
	#define stat64 _stat64
	#define lstat64 _stat64
	#define opendir _opendir
#else
	//Linux
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "exception.hpp"
#include "filesystem.hpp"
namespace Box{
	bool FS::Exists(const char *pathname){
		//判断文件是否存在
		if(access(pathname,F_OK) == 0){
			return true;
		}
		else{
			return false;
		}
	}
	void FS::MkDir(const char *name){
		//创建文件夹
		if(mkdir(name,S_IRWXU) != 0){
			throw OSError(errno,strerror(errno),name);
		}
	}
	void FS::Chdir(const char *path){
		//改变工作目录
		if(chdir(path) != 0){
			//失败
			throw OSError(errno,strerror(errno),path);
		}
	}
	//得到文件大小
	size_t FS::GetSize(const char *pathname){
		struct stat64 f_stat;//文件状态
		if(lstat64(pathname,&f_stat) != 0){
			//失败
			throw OSError(errno,strerror(errno),pathname);
		}
		return f_stat.st_size;
	}
	std::string FS::Getcwd(){
		char *cwd = getcwd(nullptr,0);
		//得到工作目录
		if(cwd == nullptr){
			//失败
			throw OSError(errno);
		}
		std::string s(cwd);
		free(cwd);
		return s;
	}
	//得到状态
	void FS::GetStat(const char *pathname,FS::Stat &st){
		if(lstat64(pathname,&st) != 0){
			//得到stat
			throw OSError(errno,strerror(errno),pathname);
		}
	}
	FS::Stat FS::GetStat(const char *pathname){
		Stat s;
		FS::GetStat(pathname,s);
		return s;
	}
	//得到错误
	const char *FS::GetError(){
		return strerror(errno);
	}
	//Stat的操作
	bool FS::Stat::is_reg() const{
		//是普通文件
		return S_ISREG(st_mode);
	}
	bool FS::Stat::is_dir() const{
		return S_ISDIR(st_mode);
	}
	bool FS::Stat::is_fifo() const{
		//是管道
		return S_ISFIFO(st_mode);
	}
	//FILE
	//从文件指针构建
	RefPtr<File> File::From(FILE *fptr){
		if(fptr == nullptr){
			//如果是空指针
			throw NullPtrException();
		}
		return new File(fptr);
	}
	//打开文件
	RefPtr<File> File::Open(const std::string &name,const std::string &mode){
		FILE *fptr = fopen(name.c_str(),mode.c_str());
		if(fptr == nullptr){
			//失败
			throw OSError(errno,strerror(errno),name.c_str());
		}
		return new File(fptr);
	}
	//临时文件
	RefPtr<File> File::Tmpfile(){
		return File::From(tmpfile());
	}
	File::File(FILE *fptr){
		this->fptr = fptr;
	}
	File::~File(){
		if(fptr != nullptr){
			fclose(fptr);
		}
	}
	//方法
	void File::close(){
		//关闭
		if(fptr != nullptr){
			fclose(fptr);
		}
	}
	//文件末尾
	bool File::eof(){
		return feof(fptr) != 0;
	}
	//是否正常
	bool File::ok(){
		return !ferror(fptr);
	}
	void File::clear(){
		clearerr(fptr);
	}
	//得到错误
	const char *File::get_error(){
		return strerror(errno);
	}
	//格式化输入输出
	int File::print(const char *fmt,...){
		va_list varg;
		va_start(varg,fmt);
		int val = this->print(fmt,varg);
		va_end(varg);
		return val;
	}
	int File::print(const char *fmt,va_list varg){
		return fprintf(fptr,fmt,varg);
	}
	int File::scan(const char *fmt,...){
		va_list varg;
		va_start(varg,fmt);
		int val = this->scan(fmt,varg);
		va_end(varg);
		return val;
	}
	int File::scan(const char *fmt,va_list varg){
		return fscanf(fptr,fmt,varg);
	}
	//读入和写入
	size_t File::read(void *buf,size_t bufsize){
		return fread(buf,1,bufsize,fptr);
	}
	size_t File::write(const void *buf,size_t bufsize){
		return fwrite(buf,1,bufsize,fptr);
	}
	//分离
	FILE *File::detach(){
		if(fptr == nullptr){
			throw NullPtrException();
		}
		FILE *f = fptr;
		fptr = nullptr;
		return f;
	}
	bool File::readline(std::string &str){
		int ch;
		ch = fgetc(fptr);
		while(ch != EOF or ch != '\n'){
			str += ch;
			ch = fgetc(fptr);
		}
		if(ferror(fptr)){
			//发生错误
			return false;
		}
		return true;
	}
};
