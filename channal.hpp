#ifndef _BOX_CHANNAL_HPP_
#define _BOX_CHANNAL_HPP_
//多线程同步用的通道
#include <cstddef>
#include <string>
namespace Box{
	namespace Sync{
		//通道
		namespace Impl{
			//实现
			//构建Socket对
			void BuildPair(void **t,void **t2);
			void Close(void *sock);
			void Read(void *sock,void *dat,size_t size);
			void Write(void *sock,const void *dat,size_t size);
			void Flush(void *sock);
			void ReadString(void *sock,std::string &s);//读入字符串
			void WriteString(void *sock,const std::string &s);//写出字符串
			void WaitForRead(void *sock);//等待直到可读
			bool ReadAble(void *sock,int ms);//可读
		};
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
				inline Port <T>(void *sock){
					this->sock = sock;
				};
				inline ~Port <T>(){
					Impl::Close(sock);
				};
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
					Impl::WaitForRead(sock);
				};
				inline bool wait(int timeout_ms){
					//等待读入 有超时版本
					return Impl::ReadAble(sock,timeout_ms);
				};
				inline bool try_read(T &t,int timeout_ms = 0);
				inline void operator >>(T &t);
				inline void operator <<(const T &t);
				inline void flush();
			private:
				void *sock;
		};
		template <class T>
		class Channal{
			public:
				Channal<T>(const Channal &) = delete;
				Channal<T>(){
					//构建两个端口
					void *socks[2];
					Impl::BuildPair(&socks[0],&socks[1]);
					_p1 = new Port<T>(socks[0]);
					_p2 = new Port<T>(socks[1]);
				};
				~Channal<T>(){
					//销毁两个
					delete _p1;
					delete _p2;
				};
				//得到两端口
				inline Port <T> &front(){
					return *_p1;
				};
				inline Port <T> &back(){
					return *_p2;
				};
			private:
				Port<T> *_p1;
				Port<T> *_p2;
		};
		//通道字符串的实现
		template <class T>
		inline void Port<T>::flush(){
			//刷新流
			Impl::Flush(sock);
		};
		template <class T>
		inline void Port<T>::operator >>(T & t){
			//读入数据
			Impl::Read(sock,&t,sizeof(t));
		};
		template <class T>
		inline void Port<T>::operator <<(const T & t){
			//写出
			Impl::Write(sock,&t,sizeof(t));
		};
		//两个函数
		//尝试读入
		template <class T>
		inline bool Port<T>::try_read(T &t,int timeout_ms){
			if(Impl::ReadAble(sock,timeout_ms)){
				Port<T>::operator >>(t);
				return true;
			}
			return false;
		};
		//字符串的特殊读入
		template <>
		inline void Port<std::string>::operator >>(std::string &s){
			s.clear();//先清空
			Impl::ReadString(sock,s);
		};
		//字符串特殊的写入
		template <>
		inline void Port<std::string>::operator <<(const std::string &s){
			Impl::WriteString(sock,s);
		};
	};
};
#endif
