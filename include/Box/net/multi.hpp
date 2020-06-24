#ifndef _BOXNET_MULTI_HPP_
#define _BOXNET_MULTI_HPP_
//多重传输
#include <functional>
#include <chrono>
namespace Box{
	namespace Net{
		class EasyPackage;
		class Easy;
		class Multi;
		struct MultiMsg{
			//Multi的信息
			Multi *multi;//Mutli的Handle
			Easy *easy;//Easy的Handle
			void *userdata;
			int code;//错误代码 等价与c得enum
			//函数
			template<class T>
			T *data() const noexcept{
				//转换内部的数据
				return static_cast<T*>(userdata);
			}
			bool ok() const noexcept;//是否可以
			void throw_for_error() const;//抛出异常如果错误
			void throw_for_status()const;//抛出异常如果Http状态代码不对
			const char *strerr()const noexcept;//得到错误
		};
		class Multi{
			//Multi接口
			public:
				Multi();
				Multi(const Multi &) = delete;
				//禁用拷贝函数
				Multi(Multi &&m);
				~Multi();
				//加入和移除曲柄(包) 这里你要自己保证曲柄的生命周期
				//Userdata是用户数据
				void add_handle(Easy &easy,void *userdata = nullptr);
				void add_handle(Easy *easy,void *userdata = nullptr);
				void add_handle(EasyPackage &pak,void *userdata = nullptr);
				void add_handle(EasyPackage *pak,void *userdata = nullptr);
				
				void remove_handle(Easy &easy);
				void remove_handle(Easy *easy);
				void remove_handle(EasyPackage &easy);
				void remove_handle(EasyPackage *easy);
				
				//int select(struct timeval &tv);//进行Select操作
				//int select();//进行Select操作 这个直接堵塞
				//会返回 -1 0 0
				int wait(int timeout_ms);//等待 直到有fd就绪发生
				int wait(const std::chrono::milliseconds &ms);
				//返回就绪的数量
				
				void perform(int &running_handles);//正在运行的handle
				int  perform();//通过返回值得到正在运行的handle
				bool get_msg(MultiMsg &msg,int &msg_in_queue);//得到信息
				template<class Fn,class ...Args>
				void for_msg(Fn &&fn,Args ...args){
					//遍历信息模板
					MultiMsg msg;
					int msg_in_queue;
					while(get_msg(msg,msg_in_queue)){
						fn(msg,args...);
					}
				};
				void for_msg(const std::function <void(const MultiMsg&)> &);//遍历信息
				//返回false表示没有信息了
			private:
				void *handle;
		};
	};
};
#endif
