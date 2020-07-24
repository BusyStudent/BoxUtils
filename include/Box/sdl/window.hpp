#ifndef _BOX_SDL_WINDOW_HPP_
#define _BOX_SDL_WINDOW_HPP_
#include <SDL2/SDL_video.h>
#include <string>
#include "rect.hpp"
namespace Box{
    namespace SDL{
        struct WindowFlags{
            //各种flags
            static constexpr Uint32 OPENGL = SDL_WINDOW_OPENGL;
            static constexpr Uint32 BORDERLESS = SDL_WINDOW_BORDERLESS;
            static constexpr Uint32 ALLOW_HIGHDPI = SDL_WINDOW_ALLOW_HIGHDPI;
        };
        class Window{
            public:
                explicit Window():win(nullptr){};
                Window(const std::string &title,int w,int h,Uint32 flags = 0);
                Window(Window &&);
                Window(const Window &) = delete;
                ~Window();
                void set_title(const std::string &title) noexcept;
                void close() noexcept;//关闭窗口
                //得到标题
                std::string title() const noexcept;
                //得到大小
                Size size() const noexcept;
                //得到包装指针
                SDL_Window *get() const noexcept{
                    return win;
                };
                //操作符号
                //赋值一个窗口指针
                Window &operator =(SDL_Window *win);
                Window &operator =(Window &&);
            private:
                SDL_Window *win;
            friend class Renderer;
        };
    };
};
#endif