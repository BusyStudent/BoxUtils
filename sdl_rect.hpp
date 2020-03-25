#ifndef _BOX_SDL_RECT_HPP_
#define _BOX_SDL_RECT_HPP_
#include <SDL2/SDL_rect.h>
//包装 SDL_Rect
//自己加了个Size
namespace Box{
	namespace SDL{
		struct Point:public SDL_Point{
			//一个点
			void operator +=(const Point &);
			void operator -=(const Point &);
			
			bool operator ==(const Point &)const;//是否相等
			bool operator !=(const Point &)const;//是否不相等
		};
		struct Size{
			//大小
			int w;
			int h;
		};
		struct Rect:public SDL_Rect{
			//移动函数
			void operator +=(const Rect&);
			void operator -=(const Rect&);
			//判断两个矩形
			bool operator ==(const Rect&) const;
			bool operator !=(const Rect&) const;
			
			bool intersect(int &x1,int &y1,int &x2,int &y2) const;
			//把线段裁剪成矩形
			//完全包含在矩形内或不相交的线段将保持不变 在两端或两端与矩形相交的线段将被剪切到矩形的边界
			//保存到参数钟
			bool intersect(const Rect&,Rect& result) const;//是否有相交的地方 第二个填充相交地方
			bool intersect(const Rect&) const;//是否有相交的地方
			bool equals(const Rect&) const;//是否相等
			bool empty() const;//是否有矩阵区域i
			
			bool enclose_points(const Point points[],int size) const;
			//如果包含点数组里面的点 那就返回true
			bool enclose_points(const Point points[],int size,Rect &result) const;
			//和上面的一样 result返回最小封闭的矩形
			
			bool has_point(const Point &) const;//判断一个点是否在矩形里面
		};
	};
};

#endif
