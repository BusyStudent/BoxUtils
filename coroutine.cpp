#include <cstdlib>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <ucontext.h>
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "exception.hpp"
using namespace Box::CRT;
#define UCXT(PTR) ((ucontext_t*)(PTR))
//转换指针的宏
size_t Coroutine::default_stack_size = 10240;
Coroutine::Coroutine(Scheduler &sche,Func fn,size_t stack_size){
	this->ucxt = malloc(sizeof(ucontext_t));//申请UCXT
	this->fn = fn;//设置函数
	
	auto ucxt = UCXT(this->ucxt);
	stack_ptr = nullptr;//设置为nullptr 等待realloc
	set_stack_size(stack_size);//申请空间
	//默认调度器为
	scheduler = &sche;
	status = CoroutineStatus::PAUSED;//默认停止的
	getcontext(ucxt);//初始化
	makecontext(ucxt,(void(*)(void))UCXT_CallFn,1,this);//设置回调
	ucxt->uc_link = UCXT(sche.ucxt);//执行完了跳到主调度器
	sche.cort_list.push_back(this);//压入自己
}
Coroutine::~Coroutine(){
	//释放资源
	free(ucxt);
	free(stack_ptr);
}
//设置栈大小
void Coroutine::set_stack_size(size_t size){
	void *nstack = realloc(stack_ptr,size);
	if(nstack == nullptr){
		//设置失败
		throw std::bad_alloc();
	}
	//设置栈
	stack_ptr = nstack;
	stack_size = size;
	//得到ucontent的 设置一下
	auto ucxt = UCXT(this->ucxt);
	ucxt->uc_stack.ss_sp = stack_ptr;//设置堆栈指针
	ucxt->uc_stack.ss_size = size;//设置大小
}
void Coroutine::UCXT_CallFn(Coroutine *self){
	//ucontext调用
	self->run();
	//切换回调度器
	self->status = CoroutineStatus::DEAD;//执行完了
	self->yield();
}
void Coroutine::run(){
	//主体的运行
	if(fn != nullptr){
		fn(*this);
	}
}
void Coroutine::set_fn(Func fn){
	this->fn = fn;
}
void Coroutine::yield(){
	//切换到调度器的片段里面
	if(swapcontext(UCXT(ucxt),UCXT(scheduler->ucxt)) == -1){
		//失败
		Box::Panic("Coroutine::yield() => %s",strerror(errno));
	}
}
//得到调度器
Scheduler *Coroutine::get_scheduler() const{
	return scheduler;
}
