#include <cstdlib>
#include <cstring>
#include <SDL2/SDL.h>
#include <map>
#include "sdl.hpp"
#include "sdl_timer.hpp"
#include "sdl_render.hpp"
#include "ui.hpp"
#include "ui_font.hpp"
#include "ui_sys.hpp"
#include "ui_window.hpp"
using namespace Box::UI;
using namespace Box;
//实例为nullptr
Sys *Sys::instance = nullptr;
Font *Sys::default_font = nullptr;
const char *Sys::default_font_name = "/usr/share/fonts-droid-fallback/truetype/DroidSansFallback.ttf";
//打开默认字体
Sys::Sys(){
	const char *err;
	default_font = Font::Open(default_font_name,12,&err);
	if(default_font == nullptr){
		Panic("Font:Open() => %s",err);
	}
}
Sys::~Sys(){
	//删除字体
	default_font->unref();
}
int Sys::main(){
	static bool running = false;//是否调用过
	
	if(running == true){
		//调用过了
		return 1;
	}
	start_loop();
	running = false;
	return 0;
}
void Sys::start_loop(){
	//主循环
	if(wins_count == 0){
		//没有窗口
		return;
	}
	SDL::Event ev;
	while(SDL::WaitEvent(ev)){
		//等待事件
		switch(ev.type){
			case SDL_WINDOWEVENT:{
				//窗口事件
				handle_window_ev(ev);
				break;
			}
			case SDL_USEREVENT:{
				//处理用户事件中这里一般是 重新画
				//计时器等不属于SDL的事件
				handle_user_ev(ev);
				break;
			}
			case SDL_MOUSEMOTION:{
				//鼠标移动
				handle_mouse_motion_ev(ev);
				break;
			}
		}
		if(wins_count == 0){
			//没有窗口了
			return;
		}
	}
}
void Sys::add_window(Uint32 wid,Window *win){
	//插入一个值
	auto ret = wins_map.insert(std::pair<Uint32,Window*>(wid,win));
	if(ret.second == false){
		//插入失败 里面有这个数值了  Panic
		Panic("std::map::insert() failed");
	}
	wins_count ++ ;
}
void Sys::remove_window(Uint32 wid){
	wins_map.erase(wid);
	wins_count -- ;
}
void Sys::handle_window_ev(Event &ev){
	win = wins_map[ev.window.windowID];
	//查找得到对应的窗口
	if(win == nullptr){
		//得到空指针
		return;
	}
	switch(ev.window.event){
		case SDL_WINDOWEVENT_CLOSE:{
			//关闭窗口
			win->close();//发送关闭窗口的消息
			break;
		}
		case SDL_WINDOWEVENT_EXPOSED:{
			//窗口暴露了 重新画图
			win->draw();
			break;
		}
	}
}
void Sys::handle_user_ev(Event &ev){
	//用户事件
	win = wins_map[ev.user.windowID];
	//查找得到对应的窗口
	if(win == nullptr){
		//得到空指针 直接忽略
		return;
	}
	SysEv code = (SysEv)ev.user.code;
	//转换一下
	switch(code){
		case SysEv::REDRAW : {
			win->draw();
			break;//重新画图
		}
		default:{
			
		}
			//未知的Code
	}
}
void Sys::handle_mouse_motion_ev(Event &ev){
	//鼠标移动事件
	win = wins_map[ev.motion.windowID];
	//查找窗口
	if(win == nullptr){
		return;
	}
	win->mouse_point.x = ev.motion.x;
	win->mouse_point.y = ev.motion.y;
	win->mouse_motion();//调用鼠标移动的函数
}
