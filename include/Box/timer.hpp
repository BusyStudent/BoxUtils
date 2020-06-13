#ifndef _BOX_TIME_HPP_
#define _BOX_TIME_HPP_
//时间的操作
#include <chrono>
namespace Box{
	namespace Time{
		void USleep(int us);
	};
	class Timer{
		public:
			/*
			Timer();
			Timer(const Timer&) = delete;
			~Timer();
			*/
			using MicroSeconds = std::chrono::microseconds;
			//回调 返回0时候会自动销毁计时器的ID 第一个是当前的计时器时间差
			typedef MicroSeconds (*Callback)(const MicroSeconds &,void *data);
			typedef const Callback *ID;//计时器ID 就是在链表里面回调指针的指针
		public:
			//一些函数
			static ID   Create(const MicroSeconds&,Callback,void *param = nullptr);
			static bool Delete(ID);//移除 失败返回nullptr
			static void Init();
			static void Quit();
		private:
			ID timer_id;//计时器ID
	};
};
#endif
