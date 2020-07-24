#include <SDL2/SDL.h>
#include "sdl/exception.hpp"
namespace Box{
namespace SDL{
    //异常
    Error::Error(const char *m):msg(m){}
    Error::~Error(){}
    //抛出异常的部分
    [[noreturn]] void throwError(){
        throw Error(SDL_GetError());
    };
    [[noreturn]] void throwError(const char *msg){
        throw Error(msg);
    };
};
};