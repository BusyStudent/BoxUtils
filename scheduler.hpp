#ifndef _BOX_SCHEDULER_HPP_
#define _BOX_SCHEDULER_HPP_
#include <list>
#include <functional>
namespace Box{
	namespace CRT{
		class Coroutine;
		class Scheduler{
			//调度器
			public:
				Scheduler();
				Scheduler(const Scheduler &) = delete;
				~Scheduler();
				void run();//运行
				virtual void push_co(Coroutine *co);//放入携程
				virtual bool is_empty();//是否空了
				Coroutine &spawn(std::function <void(Coroutine & self)>);
			protected:
				std::list <Coroutine*> cort_list;
				void *ucxt;
			friend class Coroutine;
		};
		class TScheduler:public Scheduler{
			//携程再另一条线程运行
			public:
				TScheduler();
				TScheduler(const TScheduler &) = delete;
				~TScheduler();
				void run() = delete;
				void wait_finish();//等待全部运行完毕
				bool is_empty();//是否空了
			private:
				static void ThreadMain(TScheduler *);
				void  th_main();
				Coroutine *poll_co();//尝试得到一个携程
				void push_co(Coroutine *);//压入携程
				int  wait_code();//等待代码
				void send_code(int code);//发送代码
				void *thread_ptr;
				void *mutex_ptr;
				void *event_ptr;
				void *event_finish;//没有携程可以运行了
				int _code;//代码存放地方
				std::list <Coroutine*> running_co;//正在运行保存的携程
				//继承的cort_list用于保存还没运行的
				bool is_quitting;//正在退出
		};
	};
};
#endif
