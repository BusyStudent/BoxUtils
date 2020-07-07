#ifndef _BOX_COROUTINE_HPP_
#define _BOX_COROUTINE_HPP_
#include <cstddef>
#include <functional>
#include "ucontext.hpp"
#include "def.hpp"
namespace Box{
	namespace Co{
		//协程的命名空间
		typedef std::function<void()> StartEntry;
		class Coroutinue{
			public:
				static const ssize_t StackSize = Size::KiB(12);//默认12KB
				Coroutinue(const StartEntry &entry,size_t stack_size = StackSize);
				Coroutinue(const Coroutinue &) = delete;
				~Coroutinue();
				//调用
				void operator ()();
			private:
				StartEntry entry;//入口
				uint8_t *stack_data;//栈的数据
				size_t   stack_size;
				UContext main_ctxt;
				UContext   co_ctxt;
		};
	};
};
#endif
