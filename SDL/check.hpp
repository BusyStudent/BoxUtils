#ifndef _BOX_SDL_BUILD_CHECK_HPP_
#define _BOX_SDL_BUILD_CHECK_HPP_
#include "sdl/exception.hpp"
//检查
#define SDL_CHECK(CODE) if(CODE != 0){Box::SDL::throwError();}

#endif