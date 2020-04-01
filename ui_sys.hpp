#ifndef _BOX_UI_SYS_HPP_
#define _BOX_UI_SYS_HPP_
#include <map>
#include "sdl.hpp"
#include "sdl_font.hpp"
#include "sdl_video.hpp"
namespace Box{
	namespace UI{
		class Window;//窗口
		typedef Box::SDL::Event Event;//事件
		typedef std::map <Uint32,Window*> WindowMap;
		enum class SysEv{
			//向系统发送的用户信息
			REDRAW = -1,//重新画
			
		};
		struct Font;
		class Sys{
			//核心的部分
			public:
				Sys();
				Sys(const Sys &) = delete;
				~Sys();
				static Sys *instance;//实例
				static Font *default_font;//默认字体
				static const char *default_font_name;//默认字体的名字
				int main();//主要入口
				//WID又SDL_Window的GetID得到
				void add_window(Uint32 wid,Window *);//添加窗口
				void remove_window(Uint32 wid);//移除窗口
			private:
				void handle_window_ev(Event &);//处理SDL_Window的事件
				void handle_user_ev(Event &);//处理SDL用户的事件
				void handle_mouse_motion_ev(Event &);//处理鼠标移动事件
				void start_loop();//开始循环
				WindowMap wins_map;
				Window *win;
				Uint32 wins_count = 0;//窗口计数器
		};
	};
};
#endif
