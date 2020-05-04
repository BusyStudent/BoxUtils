#ifndef _BOX_COROUTINE_HPP_
#define _BOX_COROUTINE_HPP_
#include <cstddef>
#include <functional>
namespace Box{
	namespace CRT{
		//协程的有关函数的命名空间
		class Coroutine;
		class Scheduler;//调度器
		typedef std::function <void(Coroutine &)> Func;
		//回调函数
		enum class CoroutineStatus{
			//状态
			RUNNING,//运行中
			PAUSED,//暂停了
			DEAD//执行完了
		};
		class Coroutine{
			public:
				Coroutine(Scheduler&,Func fn = nullptr,size_t stack_size = default_stack_size);
				//默认没有回调函数 栈大小为2k
				Coroutine(const Coroutine &) = delete;
				virtual ~Coroutine();
				virtual void run();//运行
				//可以通过重载这个
				void set_fn(Func fn);//设置函数
				void set_stack_size(size_t new_size);//设置栈的大小
				Scheduler *get_scheduler() const;
				//一些操控函数
				void yield();//放弃执行 请求调度其他的协程
				void sleep(unsigned int t);//睡眠
				void usleep(int ms);//精度更高的睡眠
				
				static size_t default_stack_size;//默认栈大小
			private:
				static void UCXT_CallFn(Coroutine *);
				//ucontext的起始点
				void  *ucxt;//ucontext
				void  *stack_ptr;//栈指针
				size_t stack_size;//栈大小
				Func fn;//回调
				Scheduler *scheduler;//所属的调度器
				CoroutineStatus status;//默认状态
				int max_hit;//最大可以命中的 
			friend class Scheduler;
			friend class TScheduler;
		};
	};
};
#endif
