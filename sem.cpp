#ifdef _WIN32
	#include <windows.h>
#else
	//Linux的信号量
	#include <semaphore.h>
	//#include <sys/sem.h>
#endif
#include <cstring>
#include <cstdio>
#include <cerrno>
#include "exception.hpp"
#include "sem.hpp"
using namespace Box;
Sem::Sem(unsigned int value){
	if(sem_init(&sem,0,value) !=0 ){
		//失败了
		Box::Panic("sem_init() => %s",strerror(errno));
	}
}
Sem::~Sem(){
	//信号量销毁
	 sem_destroy(&sem);
}
void Sem::wait(){
	sem_wait(&sem);
}
void Sem::post(){
	sem_post(&sem);
}
bool Sem::try_wait(){
	if(sem_trywait(&sem) ==0){
		return true;
	}
	else{
		return false;
	}
}
int Sem::get_value(){
	int val;
	sem_getvalue(&sem,&val);
	return val;
}
//锁
Mutex::Mutex():Sem(1){
	//锁初始化值为1
}
void Mutex::lock(){
	//减去信号1
	this->wait();
}
void Mutex::unlock(){
	//加上1
	this->post();
}
//事件
ThreadEvent::ThreadEvent():Sem(0){
	//默认是0
}
void ThreadEvent::set(){
	mutex.lock();
	_is_set = true;//已经被设置了
	for (int i = 0; i < sleepers; i++){
		Sem::post();
	}
	sleepers = 0;
	mutex.unlock();
}
void ThreadEvent::wait(){
	mutex.lock();
	bool status = _is_set;
	//获得状态
	if(status == true){
		mutex.unlock();
		return;
	}
	//已经被设置了
	else{
		sleepers++;
		mutex.unlock();//释放锁
		Sem::wait();
	}
}
void ThreadEvent::clear(){
	//清除状态
	mutex.lock();
	_is_set = false;//没有被设置
	mutex.unlock();
}
bool ThreadEvent::is_set(){
	return _is_set;
}
