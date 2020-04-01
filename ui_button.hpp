#ifndef _BOX_UI_BUTTON_HPP_
#define _BOX_UI_BUTTON_HPP_
//按钮
#include <string>
#include "ui_font.hpp"
#include "ui_widget.hpp"
namespace Box{
	namespace UI{
		class PushButton:public Widget{
			//按压按钮
			public:
				PushButton(int x,int y,int w,int h);
				~PushButton();
				void draw(SDL::Renderer &);//画图
				void set_text(const char *text);
				//设置颜色
				//设置X Y W H
				//void set_x(int x);
				//void set_y(int y);
				//void set_w(int w);
				//void set_h(int h);
			private:
				Rect text_pos;//文本的位置
				Color bg_color;//背景颜色
				Color border_color;//边框颜色
				Color text_color;//字体颜色
				std::string text;//消息文本
				Font *font;
				Image *image;//图像的surface
				SDL::Texture *texture;//文字纹理
		};
	};
};
#endif
