#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include "sdl_pixels.hpp"
#include "sdl_font.hpp"
using namespace Box::SDL;
Font Font::Open(const char *file,int ptsize){
	TTF_Font *font = TTF_OpenFont(file,ptsize);
	if(font == nullptr){
		//打开失败
		throw std::runtime_error(TTF_GetError());
	}
	return Font(font,file,ptsize);
}
//没有异常的打开
Font *Font::Open(const char *file,int ptsize,const char **err) noexcept{
	TTF_Font *font = TTF_OpenFont(file,ptsize);
	if(font == nullptr){
		*err = TTF_GetError();
		return nullptr;
	}
	return new Font(font,file,ptsize);
}
//构造函数
Font::Font(const Font &f){
	//拷贝函数
	font = TTF_OpenFont(f.filename,f.ptsize);
	//打开
	if(font == nullptr){
		//重新打开字体失败
		fprintf(stderr,"Font::Font(const Font&) Open Font Failed %s\n",TTF_GetError());
		abort();
	}
	filename = strdup(f.filename);
	ptsize = f.ptsize;
}
Font::Font(TTF_Font *font,const char *file,int ptsize){
	this->font = font;
	this->filename = strdup(file);
	this->ptsize = ptsize;
}
Font::~Font(){
	//销毁函数
	TTF_CloseFont(font);
	//释放字体名字
	free((void*)filename);
}
//有异常版本
//字体处理
Surface Font::render_utf8_solid(const char *text,const Color &fg){
	SDL_Surface *surf = TTF_RenderUTF8_Solid(font,text,fg);
	if(surf == nullptr){
		//渲染失败
		throw std::runtime_error(TTF_GetError());
	}
	return Surface(surf);
}
//中等速度渲染
Surface Font::render_utf8_shaded(const char *text,const Color &fg,const Color &bg){
	SDL_Surface *surf = TTF_RenderUTF8_Shaded(font,text,fg,bg);
	if(surf == nullptr){
		throw std::runtime_error(TTF_GetError());
	}
	return Surface(surf);
}
//最慢
Surface Font::render_utf8_blended(const char *text,const Color &fg){
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font,text,fg);
	if(surf == nullptr){
		throw std::runtime_error(TTF_GetError());
	}
	return Surface(surf);
}
//没异常版本
//字体处理
Surface *Font::render_utf8_solid(const char *text,const Color &fg,
								const char **err) noexcept{
	SDL_Surface *surf = TTF_RenderUTF8_Solid(font,text,fg);
	if(surf == nullptr){
		//渲染失败
		*err = TTF_GetError();
		return nullptr;
	}
	return new Surface(surf);
}
//中等速度渲染
Surface *Font::render_utf8_shaded(const char *text,const Color &fg,const Color &bg,
								 const char **err) noexcept{
	SDL_Surface *surf = TTF_RenderUTF8_Shaded(font,text,fg,bg);
	if(surf == nullptr){
		*err = TTF_GetError();
		return nullptr;
	}
	return new Surface(surf);
}
//最慢
Surface *Font::render_utf8_blended(const char *text,const Color &fg,
								   const char **err) noexcept{
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font,text,fg);
	if(surf == nullptr){
		*err = TTF_GetError();
		return nullptr;
	}
	return new Surface(surf);
}



//是否是等宽字体
bool Font::is_fixed_width() noexcept{
	if(TTF_FontFaceIsFixedWidth(font) != 0){
		//是等宽字体
		return true;
	}
	return false;
}
//设置
void Font::set_style(int style) noexcept{
	TTF_SetFontStyle(font,style);
}
//得到样式
int  Font::get_style() noexcept{
	return TTF_GetFontStyle(font);
}
const char *Font::get_stylename() noexcept{
	return TTF_FontFaceStyleName(font);
}
const char *Font::get_familyname() noexcept{
	return TTF_FontFaceFamilyName(font);
}
