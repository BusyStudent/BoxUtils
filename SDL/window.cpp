#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <string>
#include "sdl/window.hpp"
#include "sdl/exception.hpp"

namespace Box{
namespace SDL{
    Window::Window(const std::string &title,int w,int h,Uint32 flags){
        win = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            w,
            h,
            flags    
        );
        //出错
        if(win == nullptr){
            throwError();
        }
    }
    Window::Window(Window &&w){
        //移动
        win = w.win;
        w.win = nullptr;
    };
    Window::~Window(){
        if(win != nullptr){
            SDL_DestroyWindow(win);
        }
    };
    std::string Window::title() const noexcept{
        return SDL_GetWindowTitle(win);
    };
    //设置
    void Window::set_title(const std::string &title) noexcept{
        SDL_SetWindowTitle(win,title.c_str());
    };
    void Window::close() noexcept{
        SDL_DestroyWindow(win);
        win == nullptr;
    };
    //操作符号
    //给指针
    Window &Window::operator =(SDL_Window *w){
        if(win != nullptr){
            SDL_DestroyWindow(win);
        }
        win = w;
        return *this;
    };
    Window &Window::operator =(Window &&w){
        if(win != nullptr){
            SDL_DestroyWindow(win);
        }
        win = w.win;
        w.win = nullptr;
        return *this;
    };
};
};