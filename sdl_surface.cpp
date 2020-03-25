#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include "sdl_rwops.hpp"
#include "sdl_rect.hpp"
#include "sdl_surface.hpp"
using namespace Box;
using SDL::Surface;
//这个实在是太长了
Surface::~Surface(){
	//销毁
	SDL_FreeSurface(surface);
}
Surface::Surface(const Surface &surf){
	surface = surf.surface;
	surface->refcount ++ ;
}
Surface::Surface(SDL_Surface *surf){
	surface = surf;
}
//加载BMP
Surface Surface::LoadBMP(const char *f){
	SDL_Surface *surf = SDL_LoadBMP(f);
	if(surf == nullptr){
		//加载失败
		throw std::runtime_error(SDL_GetError());
	}
	return Surface(surf);
}
Surface Surface::LoadBMP(RW &rw){
	SDL_Surface *surf = SDL_LoadBMP_RW(rw.rwops,SDL_FALSE);
	//从文件抽象载入
	if(surf == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return Surface(surf);
}
//没有异常版本
Surface *Surface::LoadBMP(RW &rw,const char **err)noexcept{
	SDL_Surface *surf = SDL_LoadBMP_RW(rw.rwops,SDL_FALSE);
	if(surf == nullptr){
		*err = SDL_GetError();
		return nullptr;
	}
	return new Surface(surf);
}
Surface *Surface::LoadBMP(const char *f,const char **err)noexcept{
	SDL_Surface *surf = SDL_LoadBMP(f);
	if(surf == nullptr){
		//加载失败
		*err = SDL_GetError();
		return nullptr;
	}
	else{
		return new Surface(surf);
	}
}
//保存为BMP
bool Surface::save_bmp(const char *f){
	return SDL_SaveBMP(surface,f);
	
}
//缝合surface
bool Surface::blit(Surface &src,const Rect *srcrect,Rect *dstrect){
	if(SDL_BlitSurface(src.surface,srcrect,surface,dstrect) == 0){
		//缝合成功
		return true;
	}
	else{
		return false;
	}
}
//填充Surface
bool Surface::fill(const Rect &rect,Uint32 color){
	//用一个矩形
	if(SDL_FillRect(surface,&rect,color) == 0){
		return true;
	}
	else{
		return false;
	}
}
bool Surface::fill(const Rect rects[],int size,Uint32 color){
	if(SDL_FillRects(surface,rects,size,color) == 0){
		return true;
	}
	return false;
}
//Color Key
bool Surface::get_colorkey(Uint32 &key){
	if(SDL_GetColorKey(surface,&key) == 0){
		return true;
	}
	return false;
}
bool Surface::set_colorkey(bool flag,Uint32 key){
	if(SDL_SetColorKey(surface,flag,key) == 0){
		return true;
	}
	else{
		return false;
	}
}
//BlendMode
bool Surface::set_blendmode(BlendMode mode){
	if(SDL_SetSurfaceBlendMode(surface,mode) == 0){
		return true;
	}
	return false;
}
bool Surface::get_blendmode(BlendMode &mode){
	if(SDL_GetSurfaceBlendMode(surface,&mode) == 0){
		return true;
	}
	return false;
}
//Alpha
bool Surface::set_alphamode(Uint8 alpha){
	if(SDL_SetSurfaceAlphaMod(surface,alpha) == 0){
		return true;
	}
	return false;
}
bool Surface::get_alphamode(Uint8 &alpha){
	if(SDL_GetSurfaceAlphaMod(surface,&alpha) == 0){
		return true;
	}
	return false;
}
//RLE
bool Surface::set_RLE(bool flag){
	if(SDL_SetSurfaceRLE(surface,flag) == 0){
		return true;
	}
	return false;
}
//Must Lock
bool Surface::must_lock(){
	return SDL_MUSTLOCK(surface);
}
//Lock
bool Surface::lock(){
	if(SDL_LockSurface(surface) == 0){
		return true;
	}
	return false;
}
void Surface::unlock(){
	SDL_UnlockSurface(surface);
}
//克隆
Surface Surface::clone(){
	//克隆Surface
	SDL_Surface *surf = SDL_ConvertSurface(surface,surface->format,0);
	//创建一个站Surface
	if(surface == nullptr){
		//出错
		throw std::runtime_error(SDL_GetError());
	}
	return Surface(surf);
}
//映射
Uint32 Surface::map_rgb(Uint8 r,Uint8 g,Uint8 b)const{
	return SDL_MapRGB(surface->format,r,g,b);
}
Uint32 Surface::map_rgba(Uint8 r,Uint8 g,Uint8 b,Uint8 a)const{
	return SDL_MapRGBA(surface->format,r,g,b,a);
}
