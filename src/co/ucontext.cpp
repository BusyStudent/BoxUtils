/*
 *      win32/64-ucontext: Unix ucontext_t operations on Windows platforms
 *      Copyright(C) 2007-2014 Panagiotis E. Hadjidoukas
 *
 *      Contact Email: phadjido@gmail.com, xdoukas@ceid.upatras.gr
 *
 *      win32/64-ucontext is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      win32/64-ucontext is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with win32/64-ucontext in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef _WIN32

#include <signal.h>

#include "common/def.hpp"
#include "threadpool.hpp"
#include "co/ucontext.h"

#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <thread>
#include <mutex>


#include "sync/pipe.hpp"
#include "sync/sem.hpp"

#include "libc/atexit.h"

namespace{
	using Box::Sync::Pipe;
	using Box::Sync::Event;

	using namespace Box;

	struct Message{
		HANDLE target;//handle to get context
		LPCONTEXT mcontext;
		int *ret;//ret value pointer
		Event *wait_ev;
		bool quit;//Quit Thread
	};
	struct HelperThread{
		std::thread th;
		Pipe<Message> pipeline;
		void run();
		//get thread context
		int get_context(LPCONTEXT ctxt);
	};
	static HelperThread *helper = nullptr;
	void quit_thread(){
		//Send to quit thread
		helper->pipeline.write(Message{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			true
		});
		helper->th.join();
		delete helper;
		helper = nullptr;
	}
	void check_init(){
		//is the thread started?
		static std::once_flag flags;
		std::call_once(flags,[](){
			helper = new HelperThread();
			helper->th = StartThread([](void*){
				helper->run();
			});
			libc::atexit_once(quit_thread);
		});
	}
	void HelperThread::run(){
		for(auto &msg:pipeline){
			if(msg.quit){
				//quit the helper thread
				return;
			}
			else{
				*(msg.ret) = GetThreadContext(msg.target,msg.mcontext);
				//Wake up
				msg.wait_ev->set();
			}
		}
	}
	int HelperThread::get_context(LPCONTEXT ctxt){
		int ret;
		Message msg{
			.target = GetCurrentThread(),
			.mcontext = ctxt,
			.ret = &ret,
			.wait_ev = new Event(),
			.quit = false
		};
		pipeline << msg;
		msg.wait_ev->wait();
		delete msg.wait_ev;
		return ret;
	}
};

extern "C"{

int getcontext(ucontext_t *ucp)
{
	check_init();
	int ret;

	/* Retrieve the full machine context */
	ucp->uc_mcontext.ContextFlags = CONTEXT_FULL;
	//ret = GetThreadContext(GetCurrentThread(), &ucp->uc_mcontext);
	ret = helper->get_context(&ucp->uc_mcontext);
	return (ret == 0) ? -1: 0;
}

int setcontext(const ucontext_t *ucp)
{
	check_init();
	int ret;
	
	/* Restore the full machine context (already set) */
	ret = SetThreadContext(GetCurrentThread(), &ucp->uc_mcontext);
	return (ret == 0) ? -1: 0;
}

int makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)
{
	check_init();
	int i;
    va_list ap;
	char *sp;

	/* Stack grows down */
	sp = (char *) (size_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size;	

	/* Reserve stack space for the arguments (maximum possible: argc*(8 bytes per argument)) */
	sp -= argc * 8;

	if ( sp < (char *)ucp->uc_stack.ss_sp) {
		/* errno = ENOMEM;*/
		return -1;
	}

	/* Set the instruction and the stack pointer */
#if defined(_X86_)
	ucp->uc_mcontext.Eip = (unsigned long long) func;
	ucp->uc_mcontext.Esp = (unsigned long long) (sp - 4);
#else
	ucp->uc_mcontext.Rip = (unsigned long long) func;
	ucp->uc_mcontext.Rsp = (unsigned long long) (sp - 40);
#endif
	/* Save/Restore the full machine context */
	ucp->uc_mcontext.ContextFlags = CONTEXT_FULL;

	/* Copy the arguments */
	va_start(ap, argc);
	for (i = 0; i<argc; i++) {
		memcpy(sp, ap, 8);
		ap += 8;
		sp += 8;
	}
	va_end(ap);

	return 0;
}

int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
	check_init();
	int ret;

	if ((oucp == NULL) || (ucp == NULL)) {
		/*errno = EINVAL;*/
		_set_errno(EINVAL);
		return -1;
	}

	ret = getcontext(oucp);
	if (ret == 0) {
		ret = setcontext(ucp);
	}
	return ret;
}

}

#endif