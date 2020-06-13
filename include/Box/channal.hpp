#ifndef _BOX_CHANNAL_HPP_
#define _BOX_CHANNAL_HPP_
//多线程同步用的通道
#include <queue>
#include <mutex>
#include "sem.hpp"
namespace Box{
	namespace Sync{
		//通道
		template <class T>
		class Port;//预先声明Port
		template <class T>
		class PortIter{
			//端口迭代器
			public:
				PortIter(Port <T> *port,bool empty){
					//默认不是空的
					this->port = port;
					this->empty = empty;
					if(empty != true){
						//是迭代器初始化的时候
						//自己前进一格
						operator ++();
					}
				};
				//这个判断迭代器是否相等
				//主要用来判断是否到达末尾
				inline bool operator !=(const PortIter<T> &iter){
					return empty != iter.empty;
				};
				inline bool operator ==(const PortIter<T> &iter){
					return empty == iter.empty;
				};
				//向前进一格
				//前置++
				inline const PortIter <T> &operator ++(){
					if(port->try_read(buf) == false){
						//读入失败
						//空了
						empty = true;
					}
					else{
						empty = false;
					}
					return *this;
				};
				//后置++
				inline const PortIter <T> operator ++(int){
					PortIter <T> temp = *this;
					if(port->try_read(buf) == false){
						//读入失败
						//空了
						empty = true;
					}
					else{
						empty = false;
					}
					return temp;
				};
				inline T& operator *(){
					//解引用
					return buf;
				};
			private:
				Port<T> *port;//绑定的端口
				T buf;//缓冲存数据的地方
				bool empty;//是否空的
		};
		template <class T>
		class Port{
			//端口
			public:
				typedef PortIter <T> iterator;//定义迭代器

				inline void read(T &t){
					operator >> (t);
				};//读入
				inline void write(const T &t){
					operator << (t);
				};//写入
				inline PortIter <T> begin(){
					//迭代器开始 不是空的
					return PortIter <T>(this,false);
				};
				inline PortIter <T> end(){
					//迭代器末尾 是空的
					 return PortIter <T>(this,true);
				};
				inline void wait(){
					//等待读入 有数据可读
					lock.lock();
					if(qe.empty()){
						lock.unlock();
						event.wait();
					}
					else{
						lock.unlock();
					}
				};
				inline bool wait(int timeout_ms){
					//等待读入 有超时版本
					//return Impl::ReadAble(sock,timeout_ms);
				};
				bool try_read(T &t){
					//尝试读入
					return true;
				};
				void operator >>(T &t){

				};
				void operator <<(const T &t){

				};
			private:
				Channal<T> &channal;
		};
		template <class T>
		class Channal{
			//通道的实现
			public:
				Channal<T>(const Channal &) = delete;
				//得到两端口
				inline Port <T> &front(){
					return _p1;
				};
				inline Port <T> &back(){
					return _p2;
				};
			private:
				Port<T> _p1;
				Port<T> _p2;
				std::queue<T> p1_queue;
				std::queue<T> p2_queue;
				std::mutex p1_mutex;
				std::mutex p2_mutex;
				Event p1_event;
				Event p2_event;
				//两个mutex
			friend class Port<T>;
		};
	};
};
#endif
