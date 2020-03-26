#ifndef _BOX_SDL_RENDER_HPP_
#define _BOX_SDL_RENDER_HPP_
//2D渲染
#include <SDL2/SDL_render.h>
#define BOXSDL_USE_GFX 1
namespace Box{
	namespace SDL{
		struct Color;
		struct Rect;
		struct Point;
		class Window;
		class Surface;
		class RW;
		typedef SDL_BlendMode BlendMode;
		struct RendererInfo:public SDL_RendererInfo{
			//渲染详情
			//查找支持纹理
			const char *get_name() const;//查找名字
			Uint32 operator [](Uint32 index) const;
		};
		class Texture{
			//纹理
			public:
				Texture(const Texture &);//不能用拷贝函数
				Texture(SDL_Texture *texture);
				~Texture();
				
				bool query(Uint32 *format,int *access,int *w,int *h) noexcept;//查询数据
				bool get_size(int *w,int *h) noexcept;//得到大小
				int get_w() noexcept;
				int get_h() noexcept;
				//设置渲染模式
				bool set_blendmode(BlendMode mode) noexcept;
				//设置alpha
				bool set_alphamod(Uint8 alpha) noexcept;
				bool set_colormod(Uint8 r,Uint8 g,Uint8 b) noexcept;//设置颜色模式
				//得到数据
				bool get_blendmode(BlendMode &mode) noexcept;
				//得到alpha
				bool get_alphamod(Uint8 &alpha) noexcept;
				bool get_colormod(Uint8 &r,Uint8 &g,Uint8 &b) noexcept;//得到颜色模式
			private:
				SDL_Texture *texture;
			friend class Renderer;
		};
		class Renderer{
			//渲染器
			public:
				Renderer(SDL_Renderer *render);
				Renderer(const Renderer &);//拷贝构造器被禁用
				~Renderer();
				
				//index是渲染驱动的标号 -1代表第一个
				static Renderer Create(Window &win,int index = -1,Uint32 flags = 0);//在窗口创建渲染器
				static Renderer CreateFrom(Surface &);//创建软件渲染器
				//有异常版本
				static Renderer *Create(Window &win,int index,Uint32 flags,
										const char **err)noexcept;//没异常版本
				static Renderer *CreateFrom(Surface &surf,const char **err)noexcept;//没异常版本
				//设置操作
				bool set_draw_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255) noexcept;
				bool set_draw_blendmode(BlendMode mode) noexcept;//设置渲染模式
				bool set_target(Texture *t = nullptr) noexcept;//设置渲染目标 nullptr改回默认目标
				//得到一些数据
				bool get_output_size(int *w,int *h) noexcept;//得到输出尺寸
				bool get_info(RendererInfo &info) noexcept;
				//一些操作
				bool clear();
				bool copy(Texture &t,const Rect *src,const Rect *dst) noexcept;//复制纹理贴进来
				bool draw_line(int x1,int y1,int x2,int y2) noexcept;//画一条线
				bool draw_lines(const Point points[],int count) noexcept;//画一堆线
				bool draw_rect(const Rect &rect) noexcept;//画一个矩形 不是实心的
				bool draw_rects(const Rect rects[],int count) noexcept;//画一堆矩形 不是实心
				bool fill_rect(const Rect &rect) noexcept;//填充矩形 实心
				bool fill_rects(const Rect rects[],int count) noexcept;//填充一堆实心的矩形
				void present() noexcept;
				
				//加载图片操作 有异常
				Texture load_bmp(const char *f);
				Texture load_bmp(RW &);
				
				Texture load_image(const char *f);//加载图片
				Texture load_image(RW &);//加载图片
				
				Texture load_from(Surface &);//从surface中创建
				
				//没异常的加载方式 需要手动接受err字符串
				Texture *load_bmp(const char *f,const char **err) noexcept;
				Texture *load_bmp(RW &f,const char **err) noexcept;
				//加载图像
				Texture *load_image(const char *f,const char **err) noexcept;
				Texture *load_image(RW &,const char **err) noexcept;
				Texture *load_from(Surface &,const char **err) noexcept;
				
				Renderer *move_toheap()noexcept;//转移到堆上
				#ifdef BOXSDL_USE_GFX
				//SDL_gfx的操作
				//画一个三角形 只是连线而已
				bool gfx_draw_trigon(const Point &p1,const Point &p2,const Point &p3,
					 Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255) noexcept;
				bool gfx_draw_trigon(const Point &p1,const Point &p2,const Point &p3,
					 const Color &color) noexcept;
				//填充三角形
				bool gfx_fill_trigon(const Point &p1,const Point &p2,const Point &p3,
					 Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255) noexcept;
				bool gfx_fill_trigon(const Point &p1,const Point &p2,const Point &p3,
					 const Color &color) noexcept;
				//画一条锯齿的线条
				bool gfx_draw_aaline(int x1,int y1,int x2,int y2,
					 Uint8 r,Uint8 g,Uint8 b,Uint8 a)noexcept;
				bool gfx_draw_aaline(int x1,int y1,int x2,int y2,
					 const Color &color)noexcept;
				#endif
			private:
				SDL_Renderer *render;
		};
	};
};
#endif
