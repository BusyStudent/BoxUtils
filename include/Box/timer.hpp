#ifndef _BOX_TIME_HPP_
#define _BOX_TIME_HPP_
//时间的操作
#include <chrono>
#include <functional>
#include "libc/attr.h"
namespace Box{
	namespace Time{
		void USleep(int us);
	};
	class BOXAPI Timer{
		public:
			/*
			Timer();
			Timer(const Timer&) = delete;
			~Timer();
			*/
			using MicroSeconds = std::chrono::microseconds;
			//回调 返回0时候会自动销毁计时器的ID 第一个是当前的计时器时间差
			typedef MicroSeconds (*Callback)(MicroSeconds t,void *data);
			typedef const Callback *ID;//计时器ID 就是在链表里面回调指针的指针
		public:
			//一些函数
			static ID   Create(
				MicroSeconds ms,
				Callback cb,
				void *param = nullptr,
				void(*delete_fn)(void *param) = nullptr
			);
			static bool Delete(ID);//移除 失败返回nullptr
			static void Init();
			static void Quit();
			//计时器类
			Timer(MicroSeconds ms,const std::function<MicroSeconds(MicroSeconds)> &);
			Timer():timer_id(nullptr){};
			Timer(const Timer &) = delete;
			Timer(Timer &&t):timer_id(t.timer_id){
				t.timer_id = nullptr;
			};
			~Timer();
		private:
			ID timer_id;//计时器ID
	};
};
#endif
