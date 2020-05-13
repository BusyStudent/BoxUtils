#ifndef _BOX_SEM_HPP_
#define _BOX_SEM_HPP_
#ifdef _WIN32
	#include <windows.h>
#else
	//Linux的信号量
	#include <semaphore.h>
	//#include <sys/sem.h>
#endif
namespace Box{
	namespace Sync{
		class Sem{
			//信号量
			public:
				Sem(unsigned int value = 0);
				//pshared代表多进程的共享
				~Sem();
				int get_value();
				bool try_wait();
				void post();
				void wait();
			private:
				#ifdef _WIN32
				HANDLE sem;
				#else
				sem_t sem;
				#endif
		};
		class Mutex:public Sem{
			//排斥锁
			public:
				Mutex();
				void lock();
				void unlock();
		};
		class Event:public Sem{
			public:
				Event();
				bool is_set();
				void wait();
				void clear();
				void set();
			private:
				bool _is_set = false;//默认没有被设置
				int sleepers = 0;
				Mutex mutex;
		};
	};
	typedef Sync::Event ThreadEvent;//兼容老式代码
};
#endif
