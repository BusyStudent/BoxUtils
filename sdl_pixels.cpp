#include <SDL2/SDL_pixels.h>
#include "sdl_pixels.hpp"
using namespace Box;
SDL::PixelFormat::PixelFormat(Uint32 pixel_f){
	//生成像素格式
	format = SDL_AllocFormat(pixel_f);
}
SDL::PixelFormat::PixelFormat(const PixelFormat &f){
	//拷贝函数
	format = f.format;
	format->refcount ++ ;
}
SDL::PixelFormat::~PixelFormat(){
	//销毁像素格式
	SDL_FreeFormat(format);
}
//得到像素名称
const char *SDL::PixelFormat::get_format_name()const{
	return SDL_GetPixelFormatName(format->format);
}
Uint32 SDL::PixelFormat::get_format()const{
	return format->format;
}
//得到各种颜色的掩码
Uint32 SDL::PixelFormat::get_Rmask()const{
	return format->Rmask;
}
Uint32 SDL::PixelFormat::get_Gmask()const{
	return format->Gmask;
}
Uint32 SDL::PixelFormat::get_Bmask()const{
	return format->Bmask;
}
Uint32 SDL::PixelFormat::get_Amask()const{
	return format->Amask;
}

//通过格式得到RGBA
void SDL::PixelFormat::get_rgb(Uint32 &pixel,Uint8 &r,Uint8 &g,Uint8 &b) const{
	SDL_GetRGB(pixel,format,&r,&g,&b);
}
void SDL::PixelFormat::get_rgba(Uint32 &pixel,Uint8 &r,Uint8 &g,Uint8 &b,Uint8 &a) const{
	SDL_GetRGBA(pixel,format,&r,&g,&b,&a);
}


//颜色
Uint32 SDL::Color::map_rgb(const PixelFormat &format)const{
	return SDL_MapRGB(format.format,r,g,b);
}
Uint32 SDL::Color::map_rgba(const PixelFormat &format)const{
	return SDL_MapRGBA(format.format,r,g,b,a);
}
const char *SDL::GetPixelFormatName(Uint32 format){
	//得到像素类型名字
	return SDL_GetPixelFormatName(format);
}
const char *SDL::GetPixelFormatName(const PixelFormat &f){
	//得到像素类型名字
	return f.get_format_name();
}
