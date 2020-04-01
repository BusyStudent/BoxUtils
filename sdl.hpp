#ifndef _BOX_SDL_HPP_
#define _BOX_SDL_HPP_
#include <SDL2/SDL_events.h>
namespace Box{
	namespace SDL{
		bool Init(Uint32 flags) noexcept;
		//默认初始化
		bool Init() noexcept;
		//初始化
		void Quit() noexcept;//退出
		typedef SDL_Event Event;
		//三个SDL的事件函数
		int PollEvent(Event &) noexcept;
		int WaitEvent(Event &) noexcept;
		int PushEvent(Event &) noexcept;
		const char *GetError() noexcept;//得到错误
	};
	namespace IMG{
		//图像的IMG初始化
		bool Init(int flags) noexcept;
		bool Init() noexcept;
		void Quit() noexcept;//退出
		const char *GetError() noexcept;
	};
	namespace TTF{
		//字体初始化
		bool Init() noexcept;
		void Quit() noexcept;
		const char *GetError() noexcept;
	};
};

#endif
