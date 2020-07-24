#ifndef _BOX_SDL_RECT_HPP_
#define _BOX_SDL_RECT_HPP_
#include <SDL2/SDL_rect.h>
namespace Box{
    namespace SDL{
        struct Point:public SDL_Point{

        };
        struct Rect:public SDL_Rect{
            //是否为空
            bool empty() const noexcept{
                return SDL_RectEmpty(this);
            };
            //是否相等
            bool equals(const SDL_Rect &r) const noexcept{
                return SDL_RectEquals(this,&r);
            };
            bool operator == (const SDL_Rect &r) const noexcept{
                return SDL_RectEquals(this,&r);
            };
            bool operator != (const SDL_Rect &r) const noexcept{
                return not SDL_RectEquals(this,&r);
            };
            //是否有点
            bool has_point(const SDL_Point &p) const noexcept{
                return SDL_PointInRect(&p,this);
            };
        };
        //大小
        struct Size{
            int w,h;
        };
        typedef Point Position;
    };
};
#endif