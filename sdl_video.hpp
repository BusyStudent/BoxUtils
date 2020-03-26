#ifndef _BOX_SDL_VIDEO_HPP_
#define _BOX_SDL_VIDEO_HPP_
#include <SDL2/SDL_video.h>
#include "sdl_surface.hpp"
namespace Box{
	namespace SDL{
		struct DisplayMode:public SDL_DisplayMode{
			int get_w() const;
			int get_h() const;
			int get_rate() const;//得到刷新速率
			Uint32 get_format() const;//得到format
			const char *get_format_name() const;//得到像素格式名称
			
		};
		//这些方法基本不抛出异常 除了那些特别写了的 
		//当初忘记写noexcept
		class Window{
			//窗口
			public:
				Window(const Window &);//拷贝函数被禁用
				~Window();
				//析构器
				static Window *GetByID(Uint32 id);//通过得到窗口通过ID
				//可能失败 那就返回nullptr
				
				//创建一个窗口(失败直接抛出异常)std::runtime_error
				//title为标题 flags为SDL_WINDOW_XXX类型 默认啥都没有
				//X Y为窗口位于屏幕的地方 给个SDL_WINDOWPOS_UNDEFINE就好啦
				//W H为窗口的宽度和长度
				static Window Create(const char *title,
									 int x,int y,int w,int h,
									 Uint32 flags = 0);
				static Window Create(const char *title,
									 int w,int h,
									 Uint32 flags = 0);
				//懒人的创建 X 和Y都给你填SDL_WINDOWPOS_UNDEFINED
				//flags 默认填0
				
				//一些方法
				void hide();//隐藏
				void show();//显示
				void raise();//直接升高 获得焦点
				
				void maximize();//最大化
				void minimize();//最小化
				void restore();//恢复
				void set_icon(const Surface &icon);//设置图像
				void set_size(int w,int h);//设置尺寸
				void set_maximum_size(int max_w,int max_h);//设置最大的尺寸
				void set_minimum_size(int min_w,int min_h);//设置最小的尺寸
				void set_data(const char *name,void *userdata);//设置用户数据
				void set_title(const char *title);//设置标题
				void set_resizeable(bool resizable);//设置可以改变大小
				void set_position(int x,int y);//设置窗口位置
				bool set_opacity(float opacity);
				//设置透明度0.0f透明 1.0f不透明
				bool set_fullscreen(Uint32 flags);//设置全屏
				//flag为 0恢复桌面模式 flag可为
				//SDL_WINDOW_FULLSCREEN ...
				
				void set_grab(bool grabbed);//设置抓取 true为鼠标被限制在窗口里面
				
				bool get_grab() const;//是否被捕获
				//得到窗口大小 两个参数可以为nullptr
				void get_maximum_size(int *max_w,int *max_h) const;//得到最大的尺寸
				void get_minimum_size(int *min_w,int *min_h) const;//得到最小的尺寸
				void get_size(int *w,int *h) const;
				Uint32 get_flags() const;//得到窗口flags
				Uint32 get_id() const;//得到窗口ID
				void *get_data(const char *name) const;//得到用户数据
				void *operator [](const char *name) const;//这个也是得到用户数据
				const char *get_title() const;//得到标题
				
				//Surface
				Surface get_surface();//得到窗口的Surface
				void update_surface();//更新窗口surface
				//得到SDL_Window
				SDL_Window *get_SDL_Window() const{
					return sdl_win;
				};
				//转移到堆上
				Window *move_toheap() noexcept;
			private:
				
				Window(SDL_Window *win);
				
				SDL_Window *sdl_win;
			friend class Renderer;//渲染器
		};
		const char *GetDisplayName(int displayindex);
		int GetNumVideoDisplays();//得到最大显示数目
		bool IsScreenSaverEnabled();//屏保是否开启
		void DisableScreenSaver();//开启屏保
		void EnableScreenSaver();//开启屏保
	};
};
#endif
