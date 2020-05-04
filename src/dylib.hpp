#ifndef _BOX_DYLIB_HPP_
#define _BOX_DYLIB_HPP_
//动态库的加载
namespace Box{
	class Dylib{
		public:
			Dylib();
			~Dylib();
			bool open(const char *lib,int mode = 0);//打开
			bool close();//关闭
			void *find(const char *symbol);//查找符号
			//得到错误
			const char *get_error() const;
		private:
			const char *err;//错误信息
			void *handle;
	};
};
#endif
