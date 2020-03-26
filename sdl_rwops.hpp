#ifndef _BOX_SDL_RWOPS_HPP_
#define _BOX_SDL_RWOPS_HPP_
//文件抽象
#include  <SDL2/SDL_rwops.h>
namespace Box{
	namespace SDL{
		class RW{
			//文件抽象IO
			public:
				~RW();
				RW(const RW &);//不能使用拷贝函数
				static RW FromMem(void *mem,int size);//从内存载入
				static RW FromConstMem(const void *mem,int size);//从只读内存载入
			private:
				//SDL文件抽象
				RW(SDL_RWops *rwops);
				SDL_RWops *rwops;
			friend class Surface;
			friend class Renderer;
		};
	};
};

#endif
