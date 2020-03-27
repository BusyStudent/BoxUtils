#ifndef _BOX_SDL_TIMER_HPP_
#define _BOX_SDL_TIMER_HPP_
#include <SDL2/SDL_timer.h>
namespace Box{
	namespace SDL{
		typedef SDL_TimerCallback TimerCallBack;
		class Timer{
			//计时器 回调会在另一条线程触发
			public:
				Timer(Uint32 interval,SDL_TimerCallback cb,void *param = nullptr);
				~Timer();
				Timer(const Timer &) = delete;
				bool stop() noexcept;//直接停止
			private:
				SDL_TimerID tid;//计时器id
		};
		#if 1 
		//控制程序的帧管理器
		class Clock{
			public:
				Clock();
				Clock(const Clock &) = delete;
				bool set_fps(Uint32 fps) noexcept;
				Uint32 tick() noexcept;//等待下一次
				int get_seted_fps()noexcept;//得到设置fps
				int get_fps() noexcept;//得到当前fps
				~Clock();
			private:
				//GFX的管理器
				void *manager;
		};
		#endif
	}
}
#endif
