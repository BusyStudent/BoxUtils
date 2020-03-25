#ifndef _BOX_SDL_SURFACE_HPP_
#define _BOX_SDL_SURFACE_HPP_
#include <SDL2/SDL_surface.h>
namespace Box{
	namespace SDL{
		typedef SDL_BlendMode BlendMode;//渲染模式
		struct Rect;//矩形
		class RW;//文件抽象
		class Surface{
			public:
				~Surface();
				Surface(const Surface &);//拷贝函数
				//就是改个引用指针
				bool save_bmp(const char *f);//保存为BMP
				//缝合surface
				
				bool get_colorkey(Uint32 &key);//得到颜色键
				bool set_colorkey(bool flag,Uint32 key);//设置颜色键
				//flag为true会把Uint32的像素设置透明
				bool set_RLE(bool flag);//开启加速 但是访问像素时候要自己锁定
				
				bool set_blendmode(BlendMode mode);//设置渲染模式
				bool get_blendmode(BlendMode &mode);//得到渲染模式
				//设置alpha
				bool set_alphamode(Uint8 alpha);
				bool get_alphamode(Uint8 &alpha);
				
				bool must_lock();//必须锁定进行访问
				bool lock();//锁定
				void unlock();//取消锁定
				//缝合填充函数
				bool blit(Surface &src,const Rect *srcrect,Rect *dstrect);
				//用rect填充
				bool fill(const Rect &rect,Uint32 color);//用矩形
				bool fill(const Rect rects[],int size,Uint32 color);//用矩形数组
				//克隆一个Surface
				Surface clone();//克隆Surface
				//第一个是树组 第二个是数组大小
				
				Uint32 map_rgb(Uint8 r,Uint8 g,Uint8 b)const;//映射RGB
				Uint32 map_rgba(Uint8 r,Uint8 g,Uint8 b,Uint8 a)const;//映射RGBA
				
				static Surface LoadBMP(const char *f);
				static Surface LoadBMP(RW &rwops);
				//加载BMP有异常版本
				static Surface *LoadBMP(RW &rwops,const char **err) noexcept;
				static Surface *LoadBMP(const char *f,const char **err) noexcept;
				//加载BMP没异常
				inline SDL_Surface *operator ->()const{
					return surface;
				};
			private:
				Surface(SDL_Surface *);
				
				SDL_Surface *surface;
			friend class Window;
		};
	};
};
#endif
