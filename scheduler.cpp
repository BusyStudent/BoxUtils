#include <cstdlib>
#include <cstring>
#include <ucontext.h>
#include <thread>
#include <mutex>
#include "scheduler.hpp"
#include "coroutine.hpp"
#include "sem.hpp"
using namespace Box::CRT;
//调度器
#define UCXT(PTR) ((ucontext_t*)(PTR))
//转换指针的宏
Scheduler::Scheduler(){
	ucxt = malloc(sizeof(ucontext_t));
}
Scheduler::~Scheduler(){
	//释放上下文
	for(auto &co:cort_list){
		//删除协程
		delete co;
	}
	free(ucxt);
}
void Scheduler::push_co(Coroutine *co){
	//普通调度器 直接压入
	cort_list.push_back(co);//压入自己
}
Coroutine &Scheduler::spawn(std::function <void(Coroutine &self)> fn){
	return *new Coroutine(*this,fn);
}
void Scheduler::run(){
	std::list <Coroutine*> :: iterator iter;
	Coroutine *co;//协程指针
	getcontext(UCXT(ucxt));
	//printf("do it");
	//fflush(stdout);
	while(cort_list.empty() == false){
		//遍历链表
		iter = cort_list.begin();
		//这里估计有问题
		while(iter != cort_list.end()){
			co = *iter;
			//切换
			swapcontext(UCXT(ucxt),UCXT(co->ucxt));
			//setcontext(UCXT(co->ucxt));
			if(co->status == CoroutineStatus::DEAD){
				//运行完成了
				delete co;
				iter = cort_list.erase(iter);
			}
			else{
				++iter;
			}
		}
	}
}
bool Scheduler::is_empty(){
	return cort_list.empty();
}
//线程安全的东西
#define MUTEX(PTR) ((std::mutex*)PTR)
#define EVENT(PTR) ((Box::ThreadEvent*)PTR)
#define THREAD(PTR) ((std::thread*)PTR)
enum{
	CODE_QUIT,//退出
	CODE_CONTINUE,//进行下一轮的迭代
};
TScheduler::TScheduler(){
	mutex_ptr = new std::mutex();
	event_ptr = new Box::ThreadEvent();//线程信号量
	event_finish = new Box::ThreadEvent();
	thread_ptr = new std::thread(ThreadMain,this);//启动线程
	
	is_quitting = false;
}
TScheduler::~TScheduler(){
	is_quitting = true;//正在退出
	send_code(CODE_QUIT);//发送退出代码
	THREAD(thread_ptr)->join();//加入
	//删除
	
	delete MUTEX(mutex_ptr);
	delete EVENT(event_ptr);
	delete EVENT(event_finish);
	delete THREAD(thread_ptr);
}
//等待代码
int TScheduler::wait_code(){
	//等待代码
	EVENT(event_ptr)->wait();
	int code =  _code;//得到代码
	EVENT(event_ptr)->clear();
	//清空信号量
	return code;
}
//发送代码
void TScheduler::send_code(int code){
	_code = code;
	EVENT(event_ptr)->set();//设置信号量
}
//压携程
void TScheduler::push_co(Coroutine *co){
	MUTEX(mutex_ptr)->lock();
	//锁住
	cort_list.push_back(co);
	MUTEX(mutex_ptr)->unlock();
	//发送continue
	send_code(CODE_CONTINUE);
}
//得到
Coroutine *TScheduler::poll_co(){
	MUTEX(mutex_ptr)->lock();
	//锁住
	Coroutine *co = nullptr;
	if(cort_list.empty() == false){
		//链表不为空
		co = *cort_list.begin();
		cort_list.pop_front();
	}
	MUTEX(mutex_ptr)->unlock();
	return co;
}
void TScheduler::wait_finish(){
	//等待完成
	EVENT(event_finish)->wait();
}
void TScheduler::ThreadMain(TScheduler *self){
	self->th_main();
}
void TScheduler::th_main(){
	//初始化上下文
	getcontext(UCXT(ucxt));
	int code;
	std::list <Coroutine*> ::iterator iter;//迭代器
	Coroutine *co;
	while(true){
		iter = running_co.begin();//从头开始调度
		while(iter != running_co.end()){
			co = *iter;//运行携程
			swapcontext(UCXT(ucxt),UCXT(co->ucxt));
			//切换
			if(co->status == CoroutineStatus::DEAD){
				//死啦
				delete co;
				iter = running_co.erase(iter);
			}
			else{
				++iter;
			}
		}
		co = poll_co();//尝试拉去出
		if(co != nullptr){
			//里面有携程
			do{
				running_co.push_front(co);
				//压到前面
				co = poll_co();
			}
			while(co != nullptr);
			EVENT(event_finish)->clear();
		}
		else{
			if(running_co.empty() == false){
				//还有的话
				continue;
			}
			//没有了 等待信息
			EVENT(event_finish)->set();//没有事件被设置
			//这里好像会有内存竞争
			if(is_quitting == true){
				//要求退出
				return;
			}
			code = wait_code();
			switch(code){
				case CODE_QUIT:{
					//退出
					return;
				}
				case CODE_CONTINUE:{
					//continue
					continue;
				}
			}
			EVENT(event_finish)->clear();//被清除
		}
	}
}
bool TScheduler::is_empty(){
	//是否空了
	return running_co.empty() and cort_list.empty();
}
