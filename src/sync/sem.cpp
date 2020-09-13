#include <cstring>
#include <cstdio>
#include <cerrno>
#include <mutex>
#include <atomic>
#include <utility>
#include <chrono>
#include "common/def.hpp"
#include "sync/sem.hpp"
#include "exception.hpp"
namespace Box{
namespace Sync{
	//事件实现
	Event::Event()
		:val_isset(false){
		//初始化没被设置
	}
	Event::~Event(){

	}
	bool Event::is_set() const noexcept{
		std::lock_guard<std::mutex> val_locker(val_mutex);
		return val_isset;
	}
	void Event::set() noexcept{
		std::lock_guard<std::mutex> val_locker(val_mutex);
		if(val_isset){
			//如果已经被设置了
			return;
		}
		//设置一下
		val_isset = true;
		cond_var.notify_all();//唤醒所有
	}
	void Event::wait() noexcept{
		val_mutex.lock();
		if(val_isset){
			val_mutex.unlock();
			return;
		}
		else{
			val_mutex.unlock();
		}
		std::mutex mtx;
		std::unique_lock<std::mutex> locker(mtx);
		cond_var.wait(locker);
		//锁定
	}
	bool Event::wait(std::chrono::microseconds ms) noexcept{
		//超时等待
		val_mutex.lock();
		if(val_isset){
			//没有锁定
			val_mutex.unlock();
			return true;
		}
		else{
			val_mutex.unlock();
		}
		//得到状态
		std::mutex mtx;
		std::unique_lock<std::mutex> locker(mtx);
		//要被锁定
		return cond_var.wait_for(locker,ms) == std::cv_status::no_timeout;
	}
	void Event::clear() noexcept{
		std::lock_guard<std::mutex> val_locker(val_mutex);
		val_isset = false;
	}
	//信号量
	Semaphore::Semaphore(unsigned int value)
		:sem_value(value){
		//初始化
	}
	Semaphore::~Semaphore(){

	}
	unsigned int Semaphore::value() const noexcept{
		std::lock_guard<std::mutex> locker(val_mutex);
		return sem_value;
	}
	//加一个数值
	void Semaphore::post() noexcept{
		//锁住所有
		std::lock_guard<std::mutex> locker(val_mutex);
		if(sem_value == 0){
			sem_value ++ ;//增加数字
			cond_var.notify_all();//唤醒所有
		}
		else{
			sem_value ++ ;
		}
	}
	void Semaphore::wait() noexcept{
		val_mutex.lock();
		if(sem_value == 0){
			//如果为0
			val_mutex.unlock();
			std::mutex mtx;
			std::unique_lock<std::mutex> locker(mtx);
			cond_var.wait(locker);
			//等待一下
		}
		//减去数值
		sem_value --;
		val_mutex.unlock();
	}
	bool Semaphore::trywait() noexcept{
		//尝试等待
		std::lock_guard<std::mutex> locker(val_mutex);
		if(sem_value == 0){
			//失败
			return false;
		}
		sem_value --;
		return true;
	}
	bool Semaphore::trywait(std::chrono::microseconds ms) noexcept{
		//尝试等待ms
		val_mutex.lock();
		if(sem_value == 0){
			val_mutex.unlock();
			std::mutex mtx;
			std::unique_lock<std::mutex> locker(mtx);
			if(cond_var.wait_for(locker,ms) == std::cv_status::timeout){
				//超时
				return false;
			};
			//尝试等待
		}
		sem_value --;
		val_mutex.unlock();
		return true;
	}
}
}
