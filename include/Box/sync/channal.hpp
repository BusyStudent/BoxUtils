#ifndef _BOX_CHANNAL_HPP_
#define _BOX_CHANNAL_HPP_
//多线程同步用的通道
#include <queue>
#include <mutex>
#include <chrono>
#include "sem.hpp"
namespace Box{
namespace Sync{
	//通道
	template<class T>
	class Channal{
		public:
			struct Port{
				//端点前端
				Port &write_port;//写操作的目标端口
				Event wait_ev;//等待事件
				mutable std::mutex mtx;//锁
				std::queue<T> data;//数据
				//函数
				//初始化
				Port(Port &p):write_port(p){}
				//写
				Port &write(T &&data){
					std::lock_guard<std::mutex> locker(write_port.mtx);
					//加锁 对后端队列
					write_port.data.push(data);
					//压入数据
					//设置事件
					write_port.wait_ev.set();
					return *this;
				};
				Port &read(T &d){
					//锁住自身的
					mtx.lock();
					while(data.empty()){
						//得到数据
						mtx.unlock();
						//等待事件
						wait_ev.wait();
						mtx.lock();
					}
					//得到前面
					d = data.front();
					//弹出
					data.pop();
					mtx.unlock();
					return *this;
				};
				//尝试读入
				bool try_read(T &d){
					std::lock_guard<std::mutex> locker(mtx);
					if(data.empty()){
						return false;
					}
					else{
						d = data.front();
						data.pop();
						return true;
					}
				}
				//尝试读入 有超时的
				template<class TimeT>
				bool try_read(T &d,TimeT &time){
					mtx.lock();
					if(data.empty()){
						//空的
						mtx.unlock();
						if(not wait_ev.wait(time)){
							//超时
							return false;
						}
						mtx.lock();
						if(data.empty()){
							//还是空的
							mtx.unlock();
							return false;
						}
					}
					d = data.front();
					data.pop();
					mtx.unlock();
					return true;
				}
				bool empty() const noexcept{
					std::lock_guard<std::mutex> locker(mtx);
					return data.empty();
				}
				//操作符号
				Port &operator <<(T &&data){
					return write(std::forward<T>(data));
				}
				Port &operator >>(T &data){
					return read(std::forward<T>(data));
				}
			};
			//前端
			Port &front() noexcept{
				return _front;
			};
			//后端
			Port &back() noexcept{
				return _back;
			};
			//初始化
			Channal():_front(_back),_back(_front){
				//初始化这个通道
			}
			Channal(const Channal &) = delete;
			~Channal(){}
		private:
			Port _front;
			Port  _back;
	};
};
};
#endif
