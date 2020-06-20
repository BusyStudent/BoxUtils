#include <chrono>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <functional>
#include "ucontext.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "exception.hpp"

#include "libc/mem.h"

namespace Box{
namespace Co{
	Coroutinue::~Coroutinue(){
		libc::free<uint8_t>(stack_data);
	}
	Coroutinue::Coroutinue(const StartEntry &ety,size_t size)
		:entry(ety),stack_size(size){
		//申请栈数据
		stack_data = libc::malloc0<uint8_t>(size);
		//设置一下
		co_ctxt.uc_link = &main_ctxt;
		co_ctxt.uc_stack.ss_size = stack_size;
		co_ctxt.uc_stack.ss_sp   = stack_data;
		co_ctxt.set_entry([](void *co){
			//调用
			static_cast<Coroutinue*>(co)->entry();
		},this); 
	}
	void Coroutinue::operator()(){
		//启动
		co_ctxt.swap(main_ctxt);
	}
};
};
