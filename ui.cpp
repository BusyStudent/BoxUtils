#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include "sdl.hpp"
#include "sdl_msgbox.hpp"
#include "ui.hpp"
#include "ui_sys.hpp"
using namespace Box;
static bool inited = false;//是否初始化过
void UI::Init(){
	//初始化
	static bool reg_quit_func = true;
	if(inited == false){
		if(SDL::Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == false){
			//失败
			UI::Panic("UI::Init() => %s",SDL::GetError());
		}
		if(TTF::Init() == false){
			UI::Panic("UI::Init() => %s",TTF::GetError());
		}
		//注册一遍清理函数
		if(reg_quit_func){
			atexit(UI::Quit);
			reg_quit_func = false;
		}
		UI::Sys::instance = new UI::Sys();
		//得到新的实例
		inited = true;//初始化过了
	}
}
void UI::Quit(){
	if(inited == true){
		//只有初始化过了才会释放资源
		delete UI::Sys::instance;//释放掉 Sys
		TTF::Quit();//退出字体渲染
		IMG::Quit();//退出图像渲染
		SDL::Quit();//退出SDL
		inited = false;
	}
}
void UI::Panic(const char *msg,...){
	//错误退出
	char buf[1024];
	va_list varg;//可变参数
	va_start(varg,msg);
	vsnprintf(buf,sizeof(buf),msg,varg);
	//输出参数
	fprintf(stderr,"Error %s",buf);
	//输出信息
	SDL::ShowMessageBox(SDL_MESSAGEBOX_ERROR,"UI::Panic()",buf);
	va_end(varg);
	abort();
}
int  UI::Main(){
	//调用循环
	return Sys::instance->main();
}
