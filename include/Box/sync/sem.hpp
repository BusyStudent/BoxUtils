#ifndef _BOX_SEM_HPP_
#define _BOX_SEM_HPP_
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "../libc/attr.h"
namespace Box{
	namespace Sync{
		class BOXAPI Semaphore{
			//信号量
			public:
				Semaphore(unsigned int value);//初始数值
				Semaphore(const Semaphore &) = delete;
				~Semaphore();
				void post() noexcept;//增加数字
				void wait() noexcept;//减去1 如果为0就等待 
				bool trywait() noexcept;//尝试减去1 如果失败就返回false
				bool trywait(std::chrono::microseconds) noexcept;
				inline void operator ++() noexcept{
					//和上面一样
					post();
				};
				inline void operator --() noexcept{
					wait();
				};
				unsigned int value() const noexcept;
			private:
				std::condition_variable cond_var;
				unsigned int sem_value;//数值
				mutable std::mutex val_mutex;//对数值的锁
		};
		class BOXAPI Event{
			//事件
			public:
				template<class T = Event>
				class Setter{
					//设置器 在离开作用执行set
					public:
						inline Setter(T &ev):event(ev){};
						Setter(const Setter&) = delete;
						~Setter(){
							//设置变量
							event.set();
						};
					private:
						T &event;
				};
				template<class T = Event>
				class Cleaner{
					//清除器
					public:
						Cleaner(T &ev):event(ev){};
						Cleaner(const Cleaner&) = delete;
						~Cleaner(){
							event.clear();
						}				
					private:
						T &event;
				};
			public:
				Event();
				Event(const Event &) = delete;
				~Event();
				bool is_set() const noexcept;//是否被设置
				void clear() noexcept;//清空重置
				void wait()  noexcept;//等待
				void set()   noexcept;//设置它 唤醒所有线程

				bool wait(std::chrono::microseconds) noexcept;//有超时的等待 超时返回false
				//返回true当已经被设置 和在时间内返回
			private:
				//条件变量
				std::condition_variable cond_var;
				bool val_isset;//是否被设置
				mutable std::mutex val_mutex;//读取和设置变量的锁
		};
	};
	typedef Sync::Event ThreadEvent;//兼容老式代码
};
#endif
