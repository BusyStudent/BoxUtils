#ifndef _BOX_FILESYSTEM_HPP_
#define _BOX_FILESYSTEM_HPP_
#include <cstdio>
//简单的文件系统操作
namespace Box{
	struct File{
		public:
			bool open(const char *filename,const char *modes = default_modes);
			//打开文件
			bool open_tmpfile();//打开临时文件
			bool close();//关闭
			const char *get_error();//得到错误
			static const char *default_modes;
		private:
			FILE *fptr;//文件指针
			int _errno;//错误代码
	};
	namespace FS{
		typedef Box::File File;
		bool Exists(const char *pathname);//文件是否存在
		bool MkDir(const char *name);//创建文件
		bool Chdir(const char *path);//改变目录
		const char *GetError();//得到错误
	};
}
#endif
