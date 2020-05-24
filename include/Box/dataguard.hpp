#ifndef _BOX_DATAGUARD_HPP_
#define _BOX_DATAGUARD_HPP_
//用来保护数据在异常发生时候被销毁
#include <cassert>
namespace Box{
	template <class T>
	class DataGuard{
		public:
			inline DataGuard(T dat,void(*fn)(T) = nullptr){
				data = dat;
				delete_fn = fn;
			};
			DataGuard(const DataGuard &) = delete;
			~DataGuard(){
				if(scoped){
					if(delete_fn != nullptr){
						delete_fn(data);
					}
				}
			};
			inline void done() noexcept{
				//取消守护
				scoped = false;
			};
			inline T&operator *() noexcept{
				return data;
			};
		private:
			T data;
			bool scoped = true;//是否保护的
			void (*delete_fn)(T);//删除函数
	};
	template <class T>
	class RefData{
		//引用的数据 主要用于封装C库的时候用到
		public:
			RefData(T data,void(*fn)(T)){
				assert(fn != nullptr);
				ptr = new Data{
					.data = data,
					.delete_fn = fn,
					.refcount = 1
				};
			}
			RefData(const RefData <T> &ref){
				//复制
				ptr = ref.ptr;
				//复制指针
				if(ptr != nullptr){
					ptr->refcount ++;
				}
			}
			RefData(RefData<T> &&ref){
				//移动函数
				ptr = ref.ptr;
				ref.ptr = nullptr;
			}
			~RefData(){
				if(ptr != nullptr){
					ptr->refcount --;
					if(ptr->refcount <= 0){
						ptr->delete_fn(ptr->data);
						delete ptr;
					}
				}
			}
			//操作符号
			inline T& operator *(){
				return ptr->data;
			}
			inline T* operator ->(){
				return &(ptr->data);
			}
			inline operator T(){
				//转换一下
				return ptr->data;
			}
			RefData<T> &operator =(RefData<T> &ref){
				//给数值语句
				if(this != &ref){
					reset();
					ptr = ref.ptr;
					if(ptr != nullptr){
						ptr->refcount ++ ;
					}
				}
				return *this;
			}
			RefData<T> &operator =(RefData<T> &&ref){
				//给个右值
				if(this != &ref){
					reset();
					ptr = ref.ptr;
					ref.ptr = nullptr;
				}
				return *this;
			}
			//重置到空的状态
			void reset(){
				if(!empty()){
					//销毁掉
					ptr->refcount --;
					if(ptr->refcount <= 0){
						ptr->delete_fn(ptr->data);
						delete ptr;
						ptr = nullptr;
					}
				}
			}
			//给一个新的数值
			void reset(T new_val,void(*fn)(T) = nullptr){
				//减去引用计数器
				if(!empty()){
					ptr->refcount --;
					if(fn == nullptr){
						fn = ptr->delete_fn;
					}
					if(ptr->refcount <= 0){
						//没有关联数据
						ptr->delete_fn(ptr->data);
						delete ptr;
						ptr = nullptr;
					}
				}
				else if(fn == nullptr){
					//如果析构函数指针为空 原来又没有数据
					throw;
				}
				//生成一份特有的
				ptr = new Data{
					.data = new_val,
					.delete_fn = fn,
					.refcount = 1
				};
			}
			inline bool empty() const noexcept{
				//是否为空
				return ptr == nullptr;
			}
			inline bool unique() const noexcept{
				//是否是唯一的
				return ptr->refcount == 1;
			}
			inline int get_count() const noexcept{
				//得到计数器
				return ptr->refcount;
			}
			inline T get() const noexcept{
				//得到数据
				return ptr->data;
			}
			inline operator T() const noexcept{
				//返回一下
				return ptr->data;
			}
		private:
			struct Data{
				T data;
				void (*delete_fn)(T);
				int refcount;
			};
		//内部数据实现
		Data *ptr;
	};
};
#endif
