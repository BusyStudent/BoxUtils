#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <stdexcept>
#include "sdl_video.hpp"
using namespace Box;
using namespace Box::SDL;
Window::Window(SDL_Window *win){
	//给个值
	sdl_win = win;
	//设置这个窗口大外指针 GetByID的时候会用到
	SDL_SetWindowData(win,"__objptr__",this);
}
Window::~Window(){
	SDL_DestroyWindow(sdl_win);
}
//窗口查找
Window *Window::GetByID(Uint32 id){
	SDL_Window *win = SDL_GetWindowFromID(id);
	if(win == nullptr){
		//没找到这个id
		return nullptr;
	}
	return (Window*)SDL_GetWindowData(win,"__objptr__");
}
//窗口的创建
Window Window::Create(const char *title,
					  int x,int y,int w,int h,
					  Uint32 flags){
	//默认创建方法
	SDL_Window *win  = SDL_CreateWindow(title,x,y,w,h,flags);
	//创建窗口
	if(win == nullptr){
		//失败
		throw std::runtime_error(SDL_GetError());
	}
	return Window(win);
}
Window Window::Create(const char *title,
					  int w,int h,
					  Uint32 flags){
	//懒人创建
	return Window::Create(title,SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								w,h,flags);
}
//方法
void Window::hide(){
	SDL_HideWindow(sdl_win);
}
void Window::show(){
	SDL_ShowWindow(sdl_win);
}
void Window::raise(){
	SDL_RaiseWindow(sdl_win);
}

void Window::maximize(){
	SDL_MaximizeWindow(sdl_win);
}
void Window::minimize(){
	SDL_MinimizeWindow(sdl_win);
}
void Window::restore(){
	SDL_RestoreWindow(sdl_win);
}
//设置用户数据
void Window::set_data(const char *name,void *userdata){
	SDL_SetWindowData(sdl_win,name,userdata);
}
//设置标题
void Window::set_title(const char *title){
	SDL_SetWindowTitle(sdl_win,title);
}
//设置可以设置大小
void Window::set_resizeable(bool resizable){
	SDL_SetWindowResizable(sdl_win,(SDL_bool)resizable);
}
void Window::set_position(int x,int y){
	//设置窗口位置
	SDL_SetWindowPosition(sdl_win,x,y);
}
bool Window::set_opacity(float opacity){
	if(SDL_SetWindowOpacity(sdl_win,opacity) == 0){
		return true;
	}
	return false;
}

//设置图像
void Window::set_icon(const Surface &icon){
	SDL_SetWindowIcon(sdl_win,icon.surface);
}

bool Window::set_fullscreen(Uint32 flags){
	//设置全屏
	if(SDL_SetWindowFullscreen(sdl_win,flags) == 0){
		return true;
	}
	return false;
}
//设置尺寸
void Window::set_minimum_size(int min_w,int min_h){
	SDL_SetWindowMinimumSize(sdl_win,min_w,min_h);
}
void Window::set_maximum_size(int max_w,int max_h){
	SDL_SetWindowMaximumSize(sdl_win,max_w,max_h);
}



//是否捕获鼠标
void Window::set_grab(bool grabbed){
	SDL_SetWindowGrab(sdl_win,(SDL_bool)grabbed);
}

void Window::get_size(int *w,int *h) const{
	SDL_GetWindowSize(sdl_win,w,h);
}
void Window::get_minimum_size(int *min_w,int *min_h) const{
	//得到最小尺寸
	SDL_GetWindowMinimumSize(sdl_win,min_w,min_h);
}
void Window::get_maximum_size(int *max_w,int *max_h) const{
	//得到最大尺寸
	SDL_GetWindowMaximumSize(sdl_win,max_w,max_h);
}

bool Window::get_grab() const{
	if(SDL_GetWindowGrab(sdl_win) == SDL_TRUE){
		return true;
	}
	return false;
}
Uint32 Window::get_flags() const{
	//得到flags
	return SDL_GetWindowFlags(sdl_win);
}
Uint32 Window::get_id() const{
	return SDL_GetWindowID(sdl_win);
};
//这两个都是得到用户数据的
void *Window::get_data(const char *name) const{
	return SDL_GetWindowData(sdl_win,name);
}
void *Window::operator [](const char *name) const{
	return SDL_GetWindowData(sdl_win,name);
}

const char *Window::get_title() const{
	//得到标题
	return SDL_GetWindowTitle(sdl_win);
};
Surface Window::get_surface(){
	SDL_Surface *surf = SDL_GetWindowSurface(sdl_win);
	//surf->refcount ++ ;
	//增加引用计数器
	return Surface(surf);
}
void Window::update_surface(){
	//更新窗口surface
	SDL_UpdateWindowSurface(sdl_win);
}
//DisplayMode
Uint32 DisplayMode::get_format() const{
	return format;
}
const char *DisplayMode::get_format_name() const{
	return SDL_GetPixelFormatName(format);
}
int DisplayMode::get_rate() const{
	return refresh_rate;
}
int DisplayMode::get_w() const{
	return w;
}
int DisplayMode::get_h() const{
	return h;
}
//一些函数
const char *SDL::GetDisplayName(int display){
	return SDL_GetDisplayName(display);
}
int SDL::GetNumVideoDisplays(){
	//得到的显示输出计数
	return SDL_GetNumVideoDisplays();
}
//屏保操作
bool SDL::IsScreenSaverEnabled(){
	if(SDL_IsScreenSaverEnabled() == SDL_TRUE){
		return true;
	}
	return false;
}
void SDL::EnableScreenSaver(){
	SDL_EnableScreenSaver();
}
void SDL::DisableScreenSaver(){
	SDL_DisableScreenSaver();
}
