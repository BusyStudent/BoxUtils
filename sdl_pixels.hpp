#ifndef _BOX_SDL_PIXELS_HPP_
#define _BOX_SDL_PIXELS_HPP_
#include <SDL2/SDL_pixels.h>
namespace Box{
	namespace SDL{
		class Surface;
		class PixelFormat{
			//像素格式
			public:
				PixelFormat(Uint32 format);
				PixelFormat(const PixelFormat&);
				~PixelFormat();
				const char *get_format_name() const;
				Uint32 get_format() const;//得到像素格式
				//得到红色的掩码
				Uint32 get_Rmask() const;
				Uint32 get_Gmask() const;
				Uint32 get_Bmask() const;
				Uint32 get_Amask() const;
				//通过格式映射rgb(a)
				void get_rgb(Uint32 &pixel,Uint8 &r,Uint8 &g,Uint8 &b) const;
				void get_rgba(Uint32 &pixel,Uint8 &r,Uint8 &g,Uint8 &b,Uint8 &a) const;
				inline SDL_PixelFormat *operator ->()const{
					return format;
				};
				inline SDL_PixelFormat *get_SDL_PixelFormat() const{
					return format;
				};//得到SDL的像素格式
			private:
				//SDL的像素格式
				SDL_PixelFormat *format;
			friend class Color;
		};
		struct Color:public SDL_Color{
			//颜色
			Uint32 map_rgb(const PixelFormat &) const;//映射
			Uint32 map_rgba(const PixelFormat &) const;
			Uint32 map_rgb(const Surface &)const;
			Uint32 map_rgba(const Surface &)const;
		};
		const char *GetPixelFormatName(Uint32 format);//得到像素名称
		const char *GetPixelFormatName(const PixelFormat &);//得到像素名称
	};
};
#endif
