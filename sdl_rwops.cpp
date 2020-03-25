#include <SDL2/SDL_rwops.h>
#include <stdexcept>
#include "sdl_rwops.hpp"
using namespace Box::SDL;
RW::RW(SDL_RWops *ops){
	rwops = ops;
}
RW::~RW(){
	//关闭文件流指针
	SDL_RWclose(rwops);
}
RW RW::FromMem(void *mem,int size){
	//从内存
	SDL_RWops * ops = SDL_RWFromMem(mem,size);
	if(ops == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return RW(ops);
}
RW RW::FromConstMem(const void *mem,int size){
	//从只读内存
	SDL_RWops * ops = SDL_RWFromConstMem(mem,size);
	if(ops == nullptr){
		throw std::runtime_error(SDL_GetError());
	}
	return RW(ops);
}
