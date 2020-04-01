#include <algorithm>
#include "sdl_video.hpp"
#include "sdl_render.hpp"
#include "sdl.hpp"
#include "ui_window.hpp"
#include "ui_widget.hpp"
#include "ui_sys.hpp"
#include "ui.hpp"
//窗口的处理
using namespace Box::UI;
using namespace Box;
bool Window::mainloop(){
	//主要事件爱
	return !Sys::instance->main();
}
Window::Window(const char *title,int w,int h){
	//创建窗口 默认可以变化大小
	sdl_win = SDL::Window::Create(title,w,h,SDL_WINDOW_RESIZABLE).move_toheap();
	Uint32 wid = sdl_win->get_id();//得到窗口ID
	Sys::instance->add_window(wid,this);
	//加入窗口
	const char *err;//错误字符串
	render = SDL::Renderer::Create(*sdl_win,-1,SDL_RENDERER_ACCELERATED,&err);
	if(render == nullptr){
		//创建失败
		Panic("Create Window Failed => %s",err);
	}
	//设置一下默认背景颜色
	bg_color.r = 239;
	bg_color.g = 240;
	bg_color.b = 241;
	bg_color.a = 255;
}
Window::~Window(){
	//移除上面的组件
	for(auto &widget:widgets_vec){
		delete widget;
	}
	//移除自己这个SDL窗口
	delete render;
	delete sdl_win;
}
void Window::close(){
	if(close_cb != nullptr){
		//如果关闭回调不为nullptr
		if(close_cb(*this,close_cb_data) == false){
			//不关闭窗口
			return;
		}
	}
	Sys::instance->remove_window(sdl_win->get_id());
	//移除自己
	delete this;
}
void Window::draw(){
	//设置背景颜色
	#ifndef NDEBUG
	puts("Window::draw()");
	#endif
	render->set_draw_color(bg_color.r,bg_color.g,bg_color.b,bg_color.a);
	render->clear();
	//遍历组件 画图
	for(auto &widget:widgets_vec){
		widget->draw(*render);
	}
	render->present();
}
void Window::redraw(){
	//发出重新画请求
	SDL::Event ev;
	ev.type = SDL_USEREVENT;
	ev.user.windowID = sdl_win->get_id();
	ev.user.code = (int)SysEv::REDRAW;
	//压入请求
	SDL::PushEvent(ev);
}
//设置回调
//窗口关闭键被按到了
void Window::set_close_cb(WindowCloseCB cb,void *data){
	close_cb = cb;
	close_cb_data = data;
}
//鼠标移动
void Window::mouse_motion(){
	
}
//添加组件
void Window::add(Widget *widget){
	//压入vec 这里怕widget重复添加
	widgets_vec.push_back(widget);
}
void Window::remove(Widget *widget){
	auto iter = std::find(widgets_vec.begin(),widgets_vec.end(),widget);
	//查找组件
	if(iter != widgets_vec.end()){
		//找到了 删除
		widgets_vec.erase(iter);
		delete *iter;
	}
}
