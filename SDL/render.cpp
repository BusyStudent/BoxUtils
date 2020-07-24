#include <SDL2/SDL.h>
#include <string>
#include "sdl/render.hpp"
#include "sdl/window.hpp"
#include "sdl/exception.hpp"

#ifndef BOX_CONFIG_NOGFX
    #include <SDL2/SDL2_gfxPrimitives.h>
#endif

#include "check.hpp"

namespace Box{
namespace SDL{
    Renderer::Renderer(Window &w,int index,Uint32 flags){
        render = SDL_CreateRenderer(w.win,index,flags);
        if(render == nullptr){
            throwError();
        }
    }
    Renderer::Renderer(Renderer &&r){
        render = r.render;
        r.render = nullptr;
    }
    Renderer::~Renderer(){
        if(render != nullptr){
            SDL_DestroyRenderer(render);
        }
    }
    void Renderer::clear(){
        SDL_RenderClear(render);
    }
    void Renderer::present(){
        SDL_RenderPresent(render);
    }
    void Renderer::set_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a){
        SDL_SetRenderDrawColor(render,r,g,b,a);
        
        #ifndef BOX_CONFIG_NOGFX
        gfx_color.r = r;
        gfx_color.g = g;
        gfx_color.b = b;
        gfx_color.a = a;
        #endif
    }
    void Renderer::set_color(const SDL_Color &color){
        SDL_SetRenderDrawColor(render,color.r,color.g,color.b,color.a);
        #ifndef BOX_CONFIG_NOGFX
        gfx_color = color;
        #endif
    }
    void Renderer::draw_rect(const SDL_Rect &rect){
        SDL_RenderDrawRect(render,&rect);
    }
    //Texture
    Texture::~Texture(){
        if(texture != nullptr){
            SDL_DestroyTexture(texture);
        }
    }
};
};