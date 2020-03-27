#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl.hpp"
using namespace Box;
bool SDL::Init(Uint32 flags) noexcept{
	if(SDL_Init(flags) == 0){
		return true;
	}
	return false;
}
bool SDL::Init() noexcept{
	return SDL::Init(SDL_INIT_VIDEO);
}

void SDL::Quit() noexcept{
	return SDL_Quit();
}
//事件
int SDL::PollEvent(Event &ev) noexcept{
	return SDL_PollEvent(&ev);
}
int SDL::WaitEvent(Event &ev) noexcept{
	return SDL_WaitEvent(&ev);
}
int SDL::PushEvent(Event &ev) noexcept{
	return SDL_PushEvent(&ev);
}
//IMG库
bool IMG::Init(int flags) noexcept{
	if(IMG_Init(flags) == 0){
		return true;
	}
	return false;
}
bool IMG::Init() noexcept{
	return IMG::Init(IMG_INIT_PNG);
}
void IMG::Quit() noexcept{
	IMG_Quit();
}
//TTF库
bool TTF::Init() noexcept{
	if(TTF_Init() == 0){
		return true;
	}
	return false;
}
void TTF::Quit() noexcept{
	TTF_Quit();
}
