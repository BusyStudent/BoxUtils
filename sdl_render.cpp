#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include "sdl_pixels.hpp"
#include "sdl_rwops.hpp"
#include "sdl_rect.hpp"
#include "sdl_video.hpp"
#include "sdl_render.hpp"
using namespace Box::SDL;
Renderer::Renderer(SDL_Renderer *render){
	this->render = render;
}
Renderer::~Renderer(){
	//销毁渲染器
	SDL_DestroyRenderer(render);
}
//创建渲染器
Renderer Renderer::Create(Window &win,int index,Uint32 flags){
	SDL_Renderer *render = SDL_CreateRenderer(win.sdl_win,index,flags);
	//创建渲染器
	if(render == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return Renderer(render);
}
Renderer Renderer::CreateFrom(Surface &surf){
	SDL_Renderer *render = SDL_CreateSoftwareRenderer(surf.surface);
	if(render == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return Renderer(render);
}
//没有异常版本的创建
Renderer *Renderer::Create(Window &win,int index,Uint32 flags,
						   const char **err) noexcept{
	SDL_Renderer *render = SDL_CreateRenderer(win.sdl_win,index,flags);
	if(render == nullptr){
		//创建失败
		*err = SDL_GetError();
		return nullptr;
	}
	return new Renderer(render);
}
Renderer *Renderer::CreateFrom(Surface &surf,const char **err) noexcept{
	SDL_Renderer *render  = SDL_CreateSoftwareRenderer(surf.surface);
	if(render == nullptr){
		*err = SDL_GetError();
		return nullptr;
	}
	return new Renderer(render);
}
//颜色设置
bool Renderer::set_draw_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a) noexcept{
	if(SDL_SetRenderDrawColor(render,r,g,b,a) == 0){
		return true;
	}
	return false;
}
//渲染绘画模式设置
bool Renderer::set_draw_blendmode(BlendMode mode) noexcept{
	if(SDL_SetRenderDrawBlendMode(render,mode) == 0){
		return true;
	}
	return false;
}
//设置渲染目标
bool Renderer::set_target(Texture *t) noexcept{
	SDL_Texture *texture = nullptr;
	if(t != nullptr){
		texture = t->texture;
	}
	if(SDL_SetRenderTarget(render,texture) == 0){
		return true;
	}
	return false;
}

//得到一些数据
//得到输出尺寸
bool Renderer::get_output_size(int *w,int *h) noexcept{
	if(SDL_GetRendererOutputSize(render,w,h) == 0){
		return true;
	}
	return false;
}
bool Renderer::get_info(RendererInfo &info) noexcept{
	//得到渲染器详情
	if(SDL_GetRendererInfo(render,&info) == 0){
		return true;
	}
	return false;
}

//具体操作
bool Renderer::clear(){
	//清空渲染器
	if(SDL_RenderClear(render) == 0){
		return true;
	}
	return false;
}
//把texture贴上去
bool Renderer::copy(Texture &t,const Rect *src,const Rect *dst) noexcept{
	if(SDL_RenderCopy(render,t.texture,src,dst) == 0){
		return true;
	}
	return false;
}
//画图操作
bool Renderer::draw_line(int x1,int y1,int x2,int y2)noexcept{
	if(SDL_RenderDrawLine(render,x1,y1,x2,y2) == 0){
		return true;
	}
	return false;
}
bool Renderer::draw_lines(const Point points[],int count) noexcept{
	if(SDL_RenderDrawLines(render,points,count) == 0){
		return true;
	}
	return false;
}
//画矩形 空心的
bool Renderer::draw_rect(const Rect &rect) noexcept{
	if(SDL_RenderDrawRect(render,&rect) == 0){
		return true;
	}
	return false;
}
//通过画一个矩形数组
bool Renderer::draw_rects(const Rect rects[],int count) noexcept{
	if(SDL_RenderDrawRects(render,rects,count) == 0){
		return true;
	}
	return false;
}
//画实心的矩形
bool Renderer::fill_rect(const Rect &rect) noexcept{
	if(SDL_RenderFillRect(render,&rect) == 0){
		return true;
	}
	return false;
}
bool Renderer::fill_rects(const Rect rects[],int count) noexcept{
	if(SDL_RenderFillRects(render,rects,count) == 0){
		return true;
	}
	return false;
}

void Renderer::present() noexcept{
	//刷新渲染器
	SDL_RenderPresent(render);
}
//渲染器加载图片
Texture Renderer::load_bmp(const char *f){
	SDL_Surface *surf = SDL_LoadBMP(f);
	if(surf == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf);
	SDL_FreeSurface(surf);
	
	return Texture(texture);
}
Texture Renderer::load_bmp(RW &rw){
	//从文件抽象中加载
	SDL_Surface *surf = SDL_LoadBMP_RW(rw.rwops,SDL_FALSE);
	if(surf == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf);
	SDL_FreeSurface(surf);
	return Texture(texture);
}
//加载图像
Texture Renderer::load_image(const char *f){
	SDL_Texture *texture = IMG_LoadTexture(render,f);
	if(texture == nullptr){
		throw std::runtime_error(IMG_GetError());
	}
	return Texture(texture);
}
//从文件抽象加载图片
Texture Renderer::load_image(RW &rw){
	SDL_Texture *texture = IMG_LoadTexture_RW(render,rw.rwops,SDL_FALSE);
	if(texture == nullptr){
		throw std::runtime_error(IMG_GetError());
	}
	return Texture(texture);
}

Texture Renderer::load_from(Surface &surf){
	//从surface中加载
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf.surface);
	if(texture == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return Texture(texture);
}
//没异常的版本
Texture *Renderer::load_bmp(const char *f,const char **err) noexcept{
	//载入BMP
	SDL_Surface *surf = SDL_LoadBMP(f);
	if(surf == nullptr){
		*err = SDL_GetError();
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf);
	SDL_FreeSurface(surf);
	return new Texture(texture);
}
Texture *Renderer::load_bmp(RW &rw,const char **err) noexcept{
	//从文件抽象载入
	SDL_Surface *surf = SDL_LoadBMP_RW(rw.rwops,SDL_FALSE);
	if(surf == nullptr){
		*err = SDL_GetError();
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf);
	SDL_FreeSurface(surf);
	return new Texture(texture);
}
//
//载入图片
Texture *Renderer::load_image(const char *f,const char **err) noexcept{
	SDL_Texture *texture = IMG_LoadTexture(render,f);
	if(texture == nullptr){
		*err = IMG_GetError();
		return nullptr;
	}
	return new Texture(texture);
}
//从文件抽象载入
Texture *Renderer::load_image(RW &rw,const char **err) noexcept{
	SDL_Texture *texture = IMG_LoadTexture_RW(render,rw.rwops,SDL_FALSE);
	if(texture == nullptr){
		*err = IMG_GetError();
		return nullptr;
	}
	return new Texture(texture);
}

//从texture加载
Texture *Renderer::load_from(Surface &surf,const char **err) noexcept{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render,surf.surface);
	if(texture == nullptr){
		*err = SDL_GetError();
		return nullptr;
	}
	return new Texture(texture);
}

//Texture
Texture::Texture(SDL_Texture *texture){
	this->texture = texture;
}
Texture::~Texture(){
	SDL_DestroyTexture(texture);
}
//查询数据
bool Texture::query(Uint32 *format,int *access,int *w,int *h) noexcept{
	if(SDL_QueryTexture(texture,format,access,w,h) == 0){
		return true;
	}
	return false;
}
//得到大小
bool Texture::get_size(int *w,int *h) noexcept{
	if(SDL_QueryTexture(texture,nullptr,nullptr,w,h) == 0){
		return true;
	}
	return false;
}
//单独得到高度
int Texture::get_h() noexcept{
	int h;
	if(SDL_QueryTexture(texture,nullptr,nullptr,nullptr,&h) == 0){
		return h;
	}
	return -1;
}
//得到宽度
int Texture::get_w() noexcept{
	int w;
	if(SDL_QueryTexture(texture,nullptr,nullptr,&w,nullptr) == 0){
		return w;
	}
	return -1;
}
//设置
bool Texture::set_blendmode(BlendMode mode) noexcept{
	if(SDL_SetTextureBlendMode(texture,mode) == 0){
		return true;
	}
	return false;
}
//设置alpha
bool Texture::set_alphamod(Uint8 alpha) noexcept{
	if(SDL_SetTextureAlphaMod(texture,alpha) == 0){
		return true;
	}
	return false;
}
//设置颜色模式
bool Texture::set_colormod(Uint8 r,Uint8 g,Uint8 b)noexcept{
	if(SDL_SetTextureColorMod(texture,r,g,b) == 0){
		return true;
	}
	return false;
}


//得到数据
//得到渲染模式
bool Texture::get_blendmode(BlendMode &mode) noexcept{
	if(SDL_GetTextureBlendMode(texture,&mode) == 0){
		return true;
	}
	return false;
}
//得到alpha
bool Texture::get_alphamod(Uint8 &alpha) noexcept{
	if(SDL_GetTextureAlphaMod(texture,&alpha) == 0){
		return true;
	}
	return false;
}
bool Texture::get_colormod(Uint8 &r,Uint8 &g,Uint8 &b) noexcept{
	if(SDL_GetTextureColorMod(texture,&r,&g,&b) == 0){
		return true;
	}
	return false;
}

//renderer info
const char *RendererInfo::get_name() const{
	return name;
}
//渲染器GFX
#ifdef BOXSDL_USE_GFX
#include <SDL2/SDL2_gfxPrimitives.h>
//画一个三角形
bool Renderer::gfx_draw_trigon(const Point &p1,const Point &p2,const Point &p3,
							   Uint8 r,Uint8 g,Uint8 b,Uint8 a) noexcept{
	if(trigonRGBA(render,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,r,g,b,a) == 0){
		return true;
	}
	return false;
}
//给个颜色
bool Renderer::gfx_draw_trigon(const Point &p1,const Point &p2,const Point &p3,
							   const Color &color) noexcept{
	if(trigonRGBA(render,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,
				  color.r,color.g,color.b,color.a) == 0){
		return true;
	}
	return false;
}

//填充三角形
bool Renderer::gfx_fill_trigon(const Point &p1,const Point &p2,const Point &p3,
							   Uint8 r,Uint8 g,Uint8 b,Uint8 a) noexcept{
	if(filledTrigonRGBA(render,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,r,g,b,a) == 0){
		return true;
	}
	return false;
}
//填充三角形
bool Renderer::gfx_fill_trigon(const Point &p1,const Point &p2,const Point &p3,
							  const Color &color) noexcept{
	if(filledTrigonRGBA(render,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,
					    color.r,color.g,color.b,color.a) == 0){
		return true;
	}
	return false;
}
//画抗锯齿曲线
bool Renderer::gfx_draw_aaline(int x1,int y1,int x2,int y2,
							   Uint8 r,Uint8 g,Uint8 b,Uint8 a) noexcept{
	if(aalineRGBA(render,x1,y1,x2,y2,r,g,b,a) == 0){
		return true;
	}
	return false;
}
bool Renderer::gfx_draw_aaline(int x1,int y1,int x2,int y2,
							   const Color &color) noexcept{
	if(aalineRGBA(render,x1,y1,x2,y2,color.r,color.g,color.b,color.a) == 0){
		return true;
	}
	return false;
}
#endif
