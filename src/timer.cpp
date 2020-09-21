#include <algorithm>
#include <ctime>
#include <chrono>
#include <list>
#include <mutex>
#include <atomic>
#include <thread>
#include "common/def.hpp"
#include "threadpool.hpp"
#include "atomic.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include "sem.hpp"
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
#include "libc/atexit.h"
using namespace Box;
namespace Box{
	void Time::USleep(int us){
		#ifdef _WIN32
		Sleep(us);
		#else
		usleep(us * 1000);
		#endif
	}
}
//计时器的实现
namespace{
	using Box::Sync::Atomic;
	using Box::Sync::Event;
	using Box::StartThread;
	struct TimerData{
		Box::Timer::Callback callback;//回调
		std::chrono::microseconds ms;//剩下的时间
		void (*delete_fn)(void *param);//删除计时器的回调
		void *param;
	};

	struct TimerImpl;
	TimerImpl *timer_impl = nullptr;
	struct TimerImpl{
		//计时器线程
		TimerImpl():running(true),th(StartThread(
			[](void *self){
				static_cast<TimerImpl*>(self)->run();
			},this
		)){

		}
		TimerImpl(const TimerImpl &) = delete;
		~TimerImpl(){
			running = false;
			event.set();
			th.join();
		}
		void run();
		static inline TimerImpl &self(){
			return *timer_impl;
		}
		void lock_timers(){
			//准备访问timers
			mtx.lock();
		}
		void unlock_timers(){
			mtx.unlock();
		}
		//内部数据
		std::list<TimerData> timers;
		Event event;//事件
		Atomic<bool> running;//是否运行
		std::mutex mtx;
		std::thread th;//线程
	};
	
};
void ::TimerImpl::run(){
	using std::chrono::steady_clock;
	BOX_DEBUG("Timer thread started");
	std::list<::TimerData>::iterator iter;
	std::chrono::microseconds sleep_ms(0);//睡眠时间
	std::chrono::microseconds zero_ms(0);
	std::chrono::microseconds dur_ms;//相隔的时间
	steady_clock::time_point timep;
	
	while(running){
		lock_timers();
		if(timers.empty()){
			//如果是空的 那就等待
			unlock_timers();
			event.wait();
			continue;
		}
		dur_ms = std::chrono::microseconds((steady_clock::now() - timep).count());
		//得到时间差
		timep = steady_clock::now();
		//遍历去调用回调
		for(iter = timers.begin();iter != timers.end();){
			TimerData &timer = *iter;
			if(timer.ms - dur_ms <= zero_ms){
				//如果会超时
				if(timer.callback != nullptr){
					timer.ms = timer.callback(timer.ms,timer.param);
				}
				//调用回调
				if(timer.ms == zero_ms){
					//取消计时器
					if(iter->delete_fn != nullptr){
						iter->delete_fn(iter->param);
					}
					iter = timers.erase(iter);
					continue;
				}
			}
			else{
				timer.ms -= dur_ms;//得到剩下时间
				if(timer.ms < sleep_ms or sleep_ms != zero_ms){
					//得到最近要唤醒的计时器时间
					sleep_ms = timer.ms;
				}
			}
			++ iter;
		}
		unlock_timers();
		event.clear();
		if(event.wait(sleep_ms)){
			//如果是被其他线程唤醒的话
			event.clear();
		}
	}
	BOX_DEBUG("Timer thread quited");
}
namespace Box{
	void Timer::Init(){
		//初始化线程
		if(timer_impl == nullptr){
			timer_impl = new ::TimerImpl();
			//注册一下退出函数
			libc::atexit_once(Timer::Quit);
		}
	}
	void Timer::Quit(){
		if(timer_impl != nullptr){
			delete timer_impl;
			timer_impl = nullptr;
		}
	}
	Timer::ID Timer::Create(MicroSeconds ms,Callback callback,void *param,void(*delete_fn)(void*)){
		//创建一个计时器
		if(callback == nullptr){
			return nullptr;
		}
		auto &impl = ::TimerImpl::self();
		impl.lock_timers();
		//压入计时器
		impl.timers.push_front(
			{
				.callback = callback,
				.ms = ms,
				.delete_fn = delete_fn,
				.param = param
			}
		);
		//得到ID
		Timer::ID id = &(impl.timers.begin()->callback);
		impl.unlock_timers();
		return id;
	}
	bool Timer::Delete(ID id){
		//删除
		if(id == nullptr){
			return false;
		}
		auto &impl = ::TimerImpl::self();
		impl.lock_timers();
		for(auto iter = impl.timers.begin() ; iter != impl.timers.end();++ iter){
			if(&(iter->callback) == id){
				//找到了
				impl.timers.erase(iter);
				//删除计数器的数据
				if(iter->delete_fn != nullptr){
					iter->delete_fn(iter->param);
				}
				impl.unlock_timers();
				return true;
			}
		}
		impl.unlock_timers();
		return false;
	}
	Timer::Timer(MicroSeconds ms,const std::function<MicroSeconds(MicroSeconds)> &fn){
		Timer::Init();//初始化一下
		timer_id = Timer::Create(
			ms,
			[](MicroSeconds ms,void *param) -> MicroSeconds{
				auto fn = static_cast<std::function<MicroSeconds(MicroSeconds)>*>(param);
				return (*fn)(ms);
			},
			new std::function<MicroSeconds(MicroSeconds)>(fn),
			[](void *param){
				delete static_cast<std::function<MicroSeconds(MicroSeconds)>*>(param);
			}
		);
	}
	Timer::~Timer(){
		Timer::Delete(timer_id);
	}
};
//C API
extern "C"{
	struct Box_timer_t{
		uint64_t (*fn)(uint64_t,void*);
		void *data;
	};
	BOXAPI void *Box_timer_create(uint64_t milliseconds,uint64_t(* fn)(uint64_t,void*),void *data){
		static std::once_flag flags;
		std::call_once(flags,[](){
			Box::Timer::Init();
		});
		return (void*)(Box::Timer::Create(
			std::chrono::milliseconds(milliseconds),
			[](std::chrono::microseconds ms,void *param) -> std::chrono::microseconds{
				Box_timer_t *timer = static_cast<Box_timer_t*>(param);
				return std::chrono::milliseconds( 
					timer->fn(std::chrono::milliseconds(ms.count()).count(),timer->data)
				);
			},
			new Box_timer_t{
				fn,
				data
			},
			[](void *param){
				delete static_cast<Box_timer_t*>(param);
			}
		));
	};
	BOXAPI int Box_timer_delete(void *timer){
		return Box::Timer::Delete((Box::Timer::ID)timer) == true;
	}
}