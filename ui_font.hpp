#ifndef _BOX_UI_FONT_HPP_
#define _BOX_UI_FONT_HPP_
#include "sdl_pixels.hpp"
namespace Box{
	namespace SDL{
		class Font;
	};
	namespace UI{
		struct Image;
		typedef Box::SDL::Color Color;
		enum class FontStyle{
			//字体样式
			BOLD,
			ITALIC,
			NORMAL,
			STRIKETHROUGH,
			UNDERLINE
		};
		struct Font{
			private:
				//默认不可以调用析勾起
				~Font();
				int refcount;
			public:
				SDL::Font *sdl_font;
				Font *set_ptsize(int ptsize);//设置字体大小
				Font *set_style(FontStyle);//设置样式
				Font *clone();//克隆字体
				Font *ref();//引用
				void unref();//不引用
				
				int get_ptsize();//得到字体大小
				FontStyle get_style();//得到样式
				const char *get_font_name();//得到字体名字
				Image *render(const char *text,const Color &);//渲染
				Image *render(const char *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255);//渲染
				
				static Font *Open(const char *f,int ptsize,const char **err = nullptr);//打开字体
				//Err是一个指向错误信息的指针
		};
	};
};
#endif
