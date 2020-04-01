#ifndef _BOX_UI_WIDGET_HPP_
#define _BOX_UI_WIDGET_HPP_
#include "sdl_rect.hpp"
#include "sdl_pixels.hpp"
namespace Box{
	namespace SDL{
		class Renderer;//渲染器
		class Texture;
	};
	namespace UI{
		typedef Box::SDL::Rect Rect;
		typedef Box::SDL::Color Color;
		struct Image;
		class Window;
		class Widget{
			//基本控件
			public:
				virtual ~Widget();
				virtual void draw(SDL::Renderer &) = 0;//画图
				virtual void set_x(int x);//设置X
				virtual void set_y(int y);//设置Y
				virtual void set_h(int h);//设置H
				virtual void set_w(int w);//设置W
			protected:
				Rect pos;//位置
				Window *master_win;//所属的窗口
			friend class Window;
		};
		
	};
};
#endif
