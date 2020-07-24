#ifndef _BOX_SDL_RENDER_HPP_
#define _BOX_SDL_RENDER_hPP_
#include <SDL2/SDL_render.h>
#include <string>
#include "rect.hpp"
namespace Box{
    namespace SDL{
        struct RendererFlags{
            static constexpr Uint32 SOFTWARE = SDL_RENDERER_SOFTWARE;
            static constexpr Uint32 ACCELERATED = SDL_RENDERER_ACCELERATED;
            static constexpr Uint32 PRESENTVSYNC = SDL_RENDERER_PRESENTVSYNC;
            static constexpr Uint32 TARGETTEXTURE = SDL_RENDERER_TARGETTEXTURE;
        };
        class Window;
        class Surface;
        class Renderer{
            public:
                Renderer(Window &,int index = -1,Uint32 flags = RendererFlags::SOFTWARE);
                Renderer(const Renderer&) = delete;
                Renderer(Renderer &&);
                ~Renderer();
                //清空
                void clear();
                void present();
                //设置画图颜色
                void set_color(const SDL_Color &color);
                void set_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255);
                //画图
                void draw_rect(const SDL_Rect &rect);
            private:
                #ifndef BOX_CONFIG_NOGFX
                //没有SDL gfx
                SDL_Color gfx_color;
                #endif
                SDL_Renderer *render;
        };
        class Texture{
            public:
                Texture(SDL_Texture *);
                Texture(Renderer &,const std::string &filename);
                ~Texture();
            private:
                
                SDL_Texture *texture;
        };
    };
};
#endif