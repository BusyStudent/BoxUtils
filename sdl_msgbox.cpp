#include <SDL2/SDL_messagebox.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include "string.hpp"
#include "sdl_video.hpp"
#include "sdl_msgbox.hpp"
using namespace Box;
using SDL::MessageBox;
//MessageBox实现
struct MSGBoxInfo{
	//消息框内部详情
	inline ~MSGBoxInfo(){
		if(buttons != nullptr){
			//Buttons是realloc生成的
			for(int i  = 0;i < buttons_size;i++){
				//释放text
				free((char*)buttons[i].text);
			}
			free(buttons);
		}
	};
	inline MSGBoxInfo(){};
	inline MSGBoxInfo(const MSGBoxInfo &info){
		//内部拷贝函数
		flags = info.flags;
		title = info.title;
		msg = info.msg;
		buttons_size = info.buttons_size;
		memcpy(&color,&(info.color),sizeof(SDL_MessageBoxColorScheme));
		//直接复制颜色
		if(info.buttons != nullptr){
			//有按钮在里面
			buttons = (SDL_MessageBoxButtonData*)
				malloc(sizeof(SDL_MessageBoxButtonData)*buttons_size);
			for(int i = 0;i < buttons_size; i++){
				buttons[i].buttonid = info.buttons[i].buttonid;
				buttons[i].flags = info.buttons[i].flags;
				//复制字符串
				buttons[i].text = strdup(info.buttons[i].text);
			}
		}
		else{
			buttons = nullptr;
		}
	};
	//Where应该为 SDL_MESSAGEBOX_COLOR_XXX
	void set_color(Uint32 where,Uint8 r,Uint8 g,Uint8 b){
		//设置颜色
		SDL_MessageBoxColor &color = this->color.colors[where];
		color.r = r;
		color.g = g;
		color.b = b;
	};
	Uint32 flags = SDL_MESSAGEBOX_INFORMATION;
	std::string title;
	std::string msg;
	int buttons_size = 0;
	SDL_MessageBoxButtonData *buttons = nullptr;
	SDL_MessageBoxColorScheme color;
};
MessageBox::MessageBox(const MessageBox &box){
	MSGBoxInfo & binfo =  *((MSGBoxInfo*)box.info);
	info = new MSGBoxInfo(binfo);
}
MessageBox::MessageBox(const char *title,const char *msg){
	//初始化一个信息框
	this->info = new MSGBoxInfo();
	auto info = (MSGBoxInfo*)this->info;
	//设置标题
	if(title != nullptr){
		info->title = title;
	}
	if(msg != nullptr){
		info->msg = msg;
	}
	//设置默认颜色
	#if 1
	set_msg_color(0,0,0);
	set_bg_color(239,240,241);
	set_button_border_color(217,215,215);
	set_button_bg_color(252,252,252);
	set_button_selected_color(39,48,41);
	#endif
}
MessageBox::~MessageBox(){
	//删除内部数据
	delete (MSGBoxInfo*)this->info;
}
int MessageBox::show(Window *win) const{
	int buttonid;//窗口的id
	SDL_Window *sdl_win = nullptr;//父亲窗口
	if(win != nullptr){
		sdl_win = win->get_SDL_Window();
	}
	auto info = (MSGBoxInfo*)this->info;
	//构建MessageBox Data
	SDL_MessageBoxData data;
	//填充数据
	data.flags = info->flags;
	data.window = sdl_win;
	data.title = info->title.c_str();
	data.message = info->msg.c_str();
	data.colorScheme = &(info->color);
	data.buttons = info->buttons;
	data.numbuttons = info->buttons_size;
	
	if(SDL_ShowMessageBox(&data,&buttonid) == 0){
		return buttonid;
	}
	return -2;//失败
}
void MessageBox::add_button(const char *text,int id,Uint32 flags){
	auto info = (MSGBoxInfo*)this->info;
	//找到内部数据
	info->buttons_size ++ ;
	//增加数据
	info->buttons = (SDL_MessageBoxButtonData*)
		realloc(info->buttons,sizeof(SDL_MessageBoxButtonData)*(info->buttons_size));
	//重新生成数组
	SDL_MessageBoxButtonData & button = (info->buttons[info->buttons_size - 1]);
	button.buttonid = id;
	button.flags = flags;
	//复制文本
	button.text = strdup(text);
}
//设置一些
void MessageBox::set_title(const char *title){
	auto info = (MSGBoxInfo*)this->info;
	info->title = title;
}
void MessageBox::set_msg(const char *msg){
	auto info = (MSGBoxInfo*)this->info;
	info->msg = msg;
}
void MessageBox::set_flags(Uint32 flags){
	auto info = (MSGBoxInfo*)this->info;
	info->flags = flags;
}
//设置颜色
void MessageBox::set_msg_color(Uint8 r,Uint8 g,Uint8 b){
	//设置信息颜色
	auto info = (MSGBoxInfo*)this->info;
	//通过宏来设置颜色
	info->set_color(SDL_MESSAGEBOX_COLOR_TEXT,r,g,b);
}
void MessageBox::set_bg_color(Uint8 r,Uint8 g,Uint8 b){
	//设置背景颜色
	auto info = (MSGBoxInfo*)this->info;
	info->set_color(SDL_MESSAGEBOX_COLOR_BACKGROUND,r,g,b);
}
void MessageBox::set_button_bg_color(Uint8 r,Uint8 g,Uint8 b){
	//设置按钮背景颜色
	auto info = (MSGBoxInfo*)this->info;
	info->set_color(SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND,r,g,b);
}
void MessageBox::set_button_border_color(Uint8 r,Uint8 g,Uint8 b){
	//设置按钮边框颜色
	auto info = (MSGBoxInfo*)this->info;
	info->set_color(SDL_MESSAGEBOX_COLOR_BUTTON_BORDER,r,g,b);
}
void MessageBox::set_button_selected_color(Uint8 r,Uint8 g,Uint8 b){
	auto info = (MSGBoxInfo*)this->info;
	info->set_color(SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED,r,g,b);
}

//简单信息框
bool SDL::ShowMessageBox(Uint32 flags,const char *title,const char *text,Window *win){
	MessageBox box(title,text);
	box.set_flags(flags);
	box.add_button("OK",1);
	if(box.show(win) == -2){
		//失败
		return false;
	}
	return true;
}
