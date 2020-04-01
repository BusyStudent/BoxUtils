#ifndef _BOX_SDL_FONT_HPP_
#define _BOX_SDL_FONT_HPP_
#include <SDL2/SDL_ttf.h>
#include "sdl_surface.hpp"
namespace Box{
	namespace SDL{
		struct Color;
		class Font{
			public:
				Font(const Font &);//可以用拷贝函数
				~Font();
				static Font  Open(const char *file,int ptsize);//打开字体
				static Font *Open(const char *file,int ptsize,const char **err) noexcept;
				//没有异常打开
				//渲染UTF8最快的
				Surface render_utf8_solid(const char *text,const Color &fg);
				//渲染中等速度
				Surface render_utf8_shaded(const char *text,const Color &fg,const Color &bg);
				//质量最好 速度最慢
				Surface render_utf8_blended(const char *text,const Color &fg);
				
				//没有异常版本
				Surface *render_utf8_solid(const char *text,const Color &fg,
										  const char **err) noexcept;
				//渲染中等速度
				Surface *render_utf8_shaded(const char *text,const  Color &fg,const Color &bg,
										   const char **err) noexcept;
				//质量最好 速度最慢
				Surface *render_utf8_blended(const char *text,const Color &fg,
											 const char **err) noexcept;
				//一些字体的设置
				//设置样式
				//为TTF_STYLE_XXX
				//是否是等宽字体
				bool is_fixed_width() noexcept;
				void set_style(int style) noexcept;
				int  get_style() noexcept;//得到样式
				const char *get_stylename() noexcept;//得到样式名字
				const char *get_familyname() noexcept;//得到字体家族名字
				const char *get_filename() noexcept;//得到字体的文件名字
				int get_ptsize() noexcept;//得到字体大小
			private:
				Font(TTF_Font *font,const char *file,int ptsize);
				const char *filename;//字体名字
				int ptsize;
				TTF_Font *font;
		};
	};
};

#endif
