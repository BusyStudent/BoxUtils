#ifndef _BOX_SCHEDULER_HPP_
#define _BOX_SCHEDULER_HPP_
#include <list>
namespace Box{
	namespace CRT{
		class Coroutine;
		class Scheduler{
			//调度器
			public:
				Scheduler();
				~Scheduler();
				void run();//运行
			private:
				std::list <Coroutine*> cort_list;
				void *ucxt;
			friend class Coroutine;
		};
	};
};
#endif
