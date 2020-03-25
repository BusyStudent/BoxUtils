#ifndef _BOX_SDL_MSGBOX_HPP_
#define _BOX_SDL_MSGBOX_HPP_
#include <SDL2/SDL_stdinc.h>
//得到Uint32
namespace Box{
	namespace SDL{
		struct Color;
		class Window;
		class MessageBox{
			public:
				MessageBox(const char *title = nullptr,const char *msg = nullptr);
				MessageBox(const MessageBox&);
				~MessageBox();
				void set_flags(Uint32 flags);//设置flags
				void set_title(const char *title);//设置标题
				void set_msg(const char *msg);//设置信息
				
				//设置颜色
				void set_bg_color(Uint8 r,Uint8 g,Uint8 b);//设置背景颜色
				void set_msg_color(Uint8 r,Uint8 g,Uint8 a);//设置信息的颜色为
				void set_button_bg_color(Uint8 r,Uint8 g,Uint8 b);//设置按钮背景颜色
				void set_button_border_color(Uint8 r,Uint8 g,Uint8 b);//设置按钮边框颜色
				void set_button_selected_color(Uint8 r,Uint8 g,Uint8 b);//设置按钮扫过的颜色
				
				void add_button(const char *text,int buttonid,Uint32 flags = 0);//添加一个按钮
				int show(Window *win = nullptr) const;//把这个信息框展示出来

			private:
				//隐藏内部详情
				void *info;
		};
		//简单MessageBox 用中文可能乱码 基于SDL_ShowSimpleMessageBox
		//这个基本用于错误报告
		//flags为SDL_MESSAGEBOX_XXX 如果不需要 直接填0
		bool ShowMessageBox(Uint32 flags,const char *title,const char *text,Window *win = nullptr);
	};
};
#endif
