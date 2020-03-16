#ifndef _BOX_FILESYSTEM_HPP_
#define _BOX_FILESYSTEM_HPP_
//简单的文件系统操作
namespace Box{
	namespace FS{
		bool Exists(const char *pathname);//文件是否存在
		bool MkDir(const char *name);//创建文件
		bool Chdir(const char *path);//改变目录
	};
}
#endif
