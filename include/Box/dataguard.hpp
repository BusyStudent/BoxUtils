#ifndef _BOX_DATAGUARD_HPP_
#define _BOX_DATAGUARD_HPP_
//用来保护数据在异常发生时候被销毁
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
};
#endif
