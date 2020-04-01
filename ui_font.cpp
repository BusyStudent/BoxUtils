#include "sdl_font.hpp"
#include "ui_font.hpp"
#include "ui_sys.hpp"
#include "ui_image.hpp"
using namespace Box::UI;
using namespace Box;
Font::~Font(){
	delete sdl_font;
}
void Font::unref(){
	//不引用
	refcount --;
	if(refcount <= 0){
		delete this;
	}
}
Font *Font::ref(){
	//引用
	refcount ++;
	return this;
}
Font *Font::Open(const char *file,int ptsize,const char **err){
	//打开SDL字体
	if(file == nullptr){
		//如果没有文件 直接引用
		auto f =  Sys::default_font->ref();
		if(f->get_ptsize() != ptsize){
			//字体大小不一样
			f = f->set_ptsize(ptsize);
		}
		return f;
	}
	SDL::Font *f;
	if(err == nullptr){
		//如果没有提供错误指针
		const char *err;
		f = SDL::Font::Open(file,ptsize,&err);
	}
	else{
		//提供错误指针
		f = SDL::Font::Open(file,ptsize,err);
	}
	if(f == nullptr){
		return nullptr;
	}
	auto font = new Font;
	font->refcount = 1;
	font->sdl_font = f;
	return font;
}
Image *Font::render(const char *text,const Color &color){
	const char *err;
	SDL::Surface *surf = sdl_font->render_utf8_blended(text,color,&err);
	if(surf == nullptr){
		return nullptr;
	}
	return new Image{
		.surf = surf,
		.refcount = 1
	};
}
Image *Font::render(const char *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a){
	SDL::Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return Font::render(text,color);
}
//得到字体大小
int Font::get_ptsize(){
	return sdl_font->get_ptsize();
}
const char *Font::get_font_name(){
	return sdl_font->get_filename();
}
Font *Font::set_ptsize(int ptsize){
	this->unref();//取消引用自己
	auto f = Font::Open(get_font_name(),ptsize);
	//打开
	return f;
}
static inline int style_to_ttfstyle(FontStyle style){
	//转化到ttf的样式
	int ret;
	switch(style){
		case FontStyle::BOLD:
			ret = TTF_STYLE_BOLD;
			break;
		case FontStyle::ITALIC:
			ret = TTF_STYLE_ITALIC;
			break;
		case FontStyle::NORMAL:
			ret = TTF_STYLE_NORMAL;
			break;
		case FontStyle::STRIKETHROUGH:
			ret = TTF_STYLE_STRIKETHROUGH;
			break;
		case FontStyle::UNDERLINE:
			ret = TTF_STYLE_UNDERLINE;
	}
	return ret;
}
Font *Font::set_style(FontStyle style){
	//设置样式
	if(refcount == 1){
		//只有一个引用这字体
		sdl_font->set_style(style_to_ttfstyle(style));
		return this;
	}
	this->unref();
	auto f = this->clone();
	f->sdl_font->set_style(style_to_ttfstyle(style));
	return f;
}
Font *Font::clone(){
	//克隆一个
	return Font::Open(get_font_name(),get_ptsize());
}
FontStyle Font::get_style(){
	//得到样式
	switch(sdl_font->get_style()){
		case TTF_STYLE_BOLD:
			return FontStyle::BOLD;
		case TTF_STYLE_ITALIC:
			return FontStyle::ITALIC;
		case TTF_STYLE_NORMAL:
			return FontStyle::NORMAL;
		case TTF_STYLE_STRIKETHROUGH:
			return FontStyle::STRIKETHROUGH;
		case TTF_STYLE_UNDERLINE:
			return FontStyle::UNDERLINE;
		default:
			//不可能
			abort();
	}
}
