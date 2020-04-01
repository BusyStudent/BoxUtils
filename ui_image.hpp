#ifndef _BOX_UI_IMAGE_HPP_
#define _BOX_UI_IMAGE_HPP_
#include <cstddef>
namespace Box{
	namespace SDL{
		class Surface;
		class Texture;
	};
	namespace UI{
		struct Image{
			//图像
			public:
				Box::SDL::Surface *surf;//
				int refcount;//引用计数器
				
				Image *ref();//引用
				void unref();//不引用
				
				int get_w();//得到宽度
				int get_h();//得到高度
				
				static Image *LoadFrom(const char *f);//从哪里加载
				static Image *LoadFrom(const void *mem,size_t size);//从哪里加载
			private:
				~Image();
			friend struct Font;
		};
	};
};
#endif
