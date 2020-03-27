#include <SDL2/SDL_timer.h>
#include "sdl_timer.hpp"
#include "sdl_render.hpp"
using namespace Box::SDL;
Timer::Timer(Uint32 interval,SDL_TimerCallback cb,void *param){
	tid = SDL_AddTimer(interval,cb,param);
}
Timer::~Timer(){
	SDL_RemoveTimer(tid);
}
bool Timer::stop() noexcept{
	if(SDL_RemoveTimer(tid) == SDL_TRUE){
		return true;
	}
	return false;
}
#ifdef BOXSDL_USE_GFX
//GFX拓展帧管理器
//Clock时钟
#include <SDL2/SDL2_framerate.h>
Clock::Clock(){
	manager = new FPSmanager;
	SDL_initFramerate((FPSmanager*) manager);
}
Clock::~Clock(){
	//删除结构
	delete (FPSmanager*)manager;
}
//设置FPS速度
bool Clock::set_fps(Uint32 fps) noexcept{
	if(SDL_setFramerate((FPSmanager*)manager,fps) == 0){
		return true;
	}
	return false;
}
Uint32 Clock::tick() noexcept{
	//延迟
	return SDL_framerateDelay((FPSmanager*)manager);
}
int Clock::get_seted_fps() noexcept{
	return SDL_getFramerate((FPSmanager*)manager);
}
int Clock::get_fps() noexcept{
	return SDL_getFramecount((FPSmanager*)manager);
}
#endif
