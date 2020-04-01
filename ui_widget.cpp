#include <string>
#include <cstdlib>
#include "sdl_render.hpp"
#include "ui_widget.hpp"
using namespace Box::UI;
//组件简单粗暴的设置坐标
void Widget::set_h(int h){
	pos.h = h;
}
void Widget::set_w(int w){
	pos.w = w;
}
void Widget::set_x(int x){
	pos.x = x;
}
void Widget::set_y(int y){
	pos.y = y;
}
Widget::~Widget(){
	puts("Widget::~Widget()");
}
//按钮
