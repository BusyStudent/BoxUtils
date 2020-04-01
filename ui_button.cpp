#include <string>
#include "sdl_surface.hpp"
#include "sdl_render.hpp"
#include "ui_widget.hpp"
#include "ui_button.hpp"
#include "ui_font.hpp"
#include "ui_image.hpp"
#include "ui.hpp"
using namespace Box::UI;
PushButton::PushButton(int x,int y,int w,int h){
	//创建一个按钮
	pos.x = x;
	pos.y = y;
	pos.w = w;
	pos.h = h;
	//设置默认颜色
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	text_color.a = 255;
	//字体颜色
	
	//边框颜色
	border_color.r = 217; 
	border_color.g = 215; 
	border_color.b = 215; 
	border_color.a = 255;
	//按钮背景颜色
	
	//bg_color.r = 239;
	bg_color.r = 252;
	//bg_color.g = 240;
	bg_color.g = 252;
	//bg_color.b = 241;
	bg_color.b = 252;
	bg_color.a = 255;
	//设置字体
	font = Font::Open(nullptr,12);
	image = nullptr;
	texture = nullptr;
}
PushButton::~PushButton(){
	font->unref();//释放字体
	if(image != nullptr){
		//释放图像
		image->unref();
	}
	delete texture;//删除纹理
	puts("PushButton::~PushButton()");
}
void PushButton::draw(SDL::Renderer &render){
	//画图
	//这里解决纹理
	if(texture == nullptr and image != nullptr){
		const char *err;//构建纹理
		texture = render.load_from(*(image->surf),&err);
		if(texture == nullptr){
			//渲染失败
			Panic(err);
		}
	}
	render.set_draw_color(bg_color);
	//画背景
	render.fill_rect(pos);
	//画文字
	
	if(texture != nullptr){
		render.copy(*texture,nullptr,&text_pos);
	}
	
	//画边框
	render.set_draw_color(border_color);
	render.draw_rect(pos);
}
void PushButton::set_text(const char *text){
	//设置文本
	if(text != nullptr){
		this->text = text;
		//渲染字体
		if(image != nullptr){
			//原来有图像了
			image->unref();
		}
		image = font->render(text,text_color);
		//渲染文字
		text_pos.x = (pos.x + pos.w / 2) - image->get_w() / 2;
		text_pos.y = (pos.y + pos.h / 2) - image->get_h() / 2;
		text_pos.w = image->get_w();
		text_pos.h = image->get_h();
	}
}
//设置X Y W H
