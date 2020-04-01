#include <cstring>
#include "sdl_surface.hpp"
#include "ui_image.hpp"
using namespace Box::UI;
using namespace Box;
Image * Image::LoadFrom(const char *f){
	//从里面加载
	const char *err;
	auto surf = SDL::Surface::LoadBMP(f,&err);
	if(surf == nullptr){
		//UI::Panic(err);
		return nullptr;
	}
	return new Image{
		.surf = surf,
		.refcount = 1
	};
}
Image::~Image(){
	delete surf;
}
//引用
Image *Image::ref(){
	refcount ++;
	return this;
}
//不引用
void Image::unref(){
	refcount --;
	if(refcount <= 0){
		delete this;
	}
}
//得到高度
int Image::get_h(){
	return (*surf)->h;
}
//得到宽度
int Image::get_w(){
	return (*surf)->w;
}
