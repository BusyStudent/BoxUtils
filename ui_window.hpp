#ifndef _BOX_UI_WINDOW_HPP_
#define _BOX_UI_WINDOW_HPP_
#include <vector>
#include "sdl_pixels.hpp"
#include "sdl_rect.hpp"
namespace Box{
	namespace SDL{
		class Window;
		class Renderer;//渲染器
	};
	namespace UI{
		class Window;
		class Widget;
		typedef Box::SDL::Color Color;//颜色
		typedef bool(*WindowCloseCB)(Window &,void*);
		//窗口关闭时候的回调
		class Window{
			//窗口
			public:
				Window(const char *title,int w,int h);
				~Window();
				bool mainloop();
				void mouse_motion();//鼠标移动
				void close();//窗口退出
				void draw();//开始画图
				void redraw();//发送重新画图事件
				void set_close_cb(WindowCloseCB cb,void *data = nullptr);//设置关闭时候的回调
				void add(Widget *widget);//添加组件
				//不要重复添加同一个组件
				void remove(Widget *widget);//移除组件
			private:
				Box::SDL::Window *sdl_win;//SDL的窗口
				Box::SDL::Renderer *render;//SDL的渲染器
				Box::SDL::Point mouse_point;
				//关闭时候触发的回调
				WindowCloseCB close_cb = nullptr;
				//这是传入的参数
				void *close_cb_data = nullptr;
				Color bg_color;//背景颜色
				//组件的Vector
				std::vector <Widget *> widgets_vec;//组件
			friend class Sys;
		};
	};
};
#endif
