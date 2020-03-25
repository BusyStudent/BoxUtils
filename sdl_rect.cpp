#include <SDL2/SDL_rect.h>
#include "sdl_rect.hpp"
using namespace Box;
//加减操作
void SDL::Rect::operator +=(const Rect &rect){
	x += rect.x;
	y += rect.y;
}
void SDL::Rect::operator -=(const Rect &rect){
	x -= rect.x;
	y -= rect.y;
}


bool SDL::Rect::operator ==(const Rect& rect)const{
	//比较两个矩形是否相等
	return SDL_RectEquals(this,&rect);
}
bool SDL::Rect::operator !=(const Rect& rect)const{
	//是否不相等
	return !SDL_RectEquals(this,&rect);
}
bool SDL::Rect::equals(const Rect& rect)const{
	return SDL_RectEquals(this,&rect);
}
bool SDL::Rect::empty()const{
	//是否有矩阵区域
	return SDL_RectEmpty(this);
}

bool SDL::Rect::intersect(int &x1,int &y1,int &x2,int &y2)const{
	return SDL_IntersectRectAndLine(this,&x1,&y1,&x2,&y2);
}
bool SDL::Rect::intersect(const Rect &rect,Rect &result)const{
	return SDL_IntersectRect(this,&rect,&result);
}
bool SDL::Rect::intersect(const Rect &rect)const{
	//判断矩形是否有相交地方
	return SDL_HasIntersection(&rect,this);
}
//Rect里面有没有这些点
bool SDL::Rect::enclose_points(const Point points[],int size)const{
	return SDL_EnclosePoints(points,size,this,nullptr);
}
bool SDL::Rect::enclose_points(const Point points[],int size,Rect &result)const{
	//Result返回矩形
	return SDL_EnclosePoints(points,size,this,&result);
}

bool SDL::Rect::has_point(const Point &point)const{
	//矩形是否有一个点
	return SDL_PointInRect(&point,this);
}
//Point的操作
bool SDL::Point::operator ==(const Point &point) const{
	//比较两个是否相等
	if(x == point.x and y == point.y){
		return true;
	}
	return false;
}
bool SDL::Point::operator !=(const Point &point) const{
	if(x == point.x and y == point.y){
		return false;
	}
	return true;
}

void SDL::Point::operator +=(const Point &point){
	x += point.x;
	y += point.y;
}
void SDL::Point::operator -=(const Point &point){
	x -= point.x;
	y -= point.y;
}

