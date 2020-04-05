#include <cstdlib>
#include <cstring>
#include <ucontext.h>
#include "scheduler.hpp"
#include "coroutine.hpp"
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
