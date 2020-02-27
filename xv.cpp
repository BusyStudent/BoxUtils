#include <pthread.h>
#include <csignal>
#include "xv.hpp"

#ifndef NDEBUG
//调试输出
#define LOG(MSG) fprintf(stderr,MSG);
#else
#define LOG(MSG,...)
#endif
using namespace BoxUtils;
static bool pop_in_vec(std::vector <Socket*> & vec,Socket *sock){
	//弹出
	auto iter = vec.begin();
	while(iter != vec.end()){
		if(*iter == sock){
			vec.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}
static inline void check_timeval(struct timeval &t1,const struct timeval &t2){
	//检查timeval
	if(t1.tv_usec != t2.tv_usec){
		t1.tv_usec = t2.tv_usec;
	}
	if(t1.tv_sec != t2.tv_sec){
		t1.tv_sec = t2.tv_sec;
	}
}
void *Xv::ThreadMain(void *handle){
	((Xv*)handle)->run();//启动事件循环
	return nullptr;
}
Xv::Xv(){
	//清空集合
	FD_ZERO(&ap_set);
	FD_ZERO(&rw_set);
	
	timeout.tv_sec = 1; 
	timeout.tv_usec = 1000; 
	pthread_mutex_init(&mutex,nullptr);
	pthread_create(&thread,nullptr,ThreadMain,this);
}
Xv::~Xv(){
	//析构时候Join线程
	pthread_cancel(thread);//关闭线程
	pthread_join(thread,nullptr);
	pthread_mutex_destroy(&mutex);
	//删除Socket
	std::vector <Socket*> ::iterator iter;
	for(iter = rw_socks.end();iter != rw_socks.end();++iter){
		delete *iter;
	}
	for(iter = ap_socks.end();iter != ap_socks.end();++iter){
		delete *iter;
	}
}
void Xv::run(){
	Event ev;
	int ret;
	unsigned int rw_count;//写入和读出的大小
	unsigned int ap_count;//ap_socks的大小
	std::vector <Socket*> :: iterator iter;
	struct timeval timeout_tmp = timeout;
	struct timeval *tptr = &(timeout_tmp);
	while(true){
		if(xv_queue.empty() !=true){
			//不为空
			pthread_mutex_lock(&mutex);
			//处理事件
			auto count = xv_queue.size();
			while(count != 0){
				ev = xv_queue.front();//获取最前面的哪一个
				xv_queue.pop();//弹掉一个
				count --;
				//解析事件
				switch(ev.type){
					case Event::XV_ADD_SOCK:
						//添加Socket再RW里面
						rw_socks.push_back(ev.sock);
						FD_SET(ev.sock->fd,&rw_set);
						//添加集合
						break;
					case Event::XV_ADD_ACCEPT_SOCK:
						//添加接入得Socket
						ap_socks.push_back(ev.sock);
						FD_SET(ev.sock->fd,&ap_set);
						break;
					case Event::XV_REMOVE_SOCK:
						pop_in_vec(rw_socks,ev.sock);
						//移除fd
						FD_CLR(ev.sock->fd,&rw_set);
						break;
					case Event::XV_REMOVE_ACCEPT_SOCK:
						//移除在连接池子
						pop_in_vec(ap_socks,ev.sock);
						FD_CLR(ev.sock->fd,&ap_set);
						break;
					case Event::XV_DELETE_SOCK:
						//删除Socket
						pop_in_vec(rw_socks,ev.sock);
						FD_CLR(ev.sock->fd,&rw_set);
						delete ev.sock;
						break;
					 case Event::XV_DELETE_ACCEPT_SOCK:
						pop_in_vec(ap_socks,ev.sock);
						FD_CLR(ev.sock->fd,&ap_set);
						delete ev.sock;
						break;
					default:
						errno = EINVAL;
						//非法参数
						goto has_error;
				}
			}
			pthread_mutex_unlock(&mutex);
		}
		ap_count = ap_socks.size();
		ret = select(ap_count,&ap_set,nullptr,nullptr,tptr);
		//等待接入
		if(ret < 0){
			//错误
			has_error:;//出错了
			ev.type = Event::XV_ERROR;
			ev.errno_ = errno;//错误代码
			push_event(ev);
			pthread_exit(nullptr);
		}
		else if(ret != 0){
			//没有超时
			iter = ap_socks.begin();//遍历表
			while(ret != 0){
				Socket &sock = **iter;
				if(iter == ap_socks.end()){
					//到末尾了站跳出
					break;
				}
				else if(FD_ISSET(sock.fd,&ap_set)){
					//如果被设置了 是这个Socket
					ev.type = Event::ACCEPT;
					ev.sock = &sock;
					ret --;
				}
				++iter;
			}
		}
		else{
			//超时
			
		}
		check_timeval(timeout_tmp,timeout);
		rw_count = rw_socks.size();
		//这里准备读取事件
		ret = select(rw_count,&rw_set,nullptr,nullptr,tptr);
		if(ret < 0){
			//error
			goto has_error;
		}
		else if(ret != 0){
			//有读取事件
			iter = rw_socks.begin();
			while(ret != 0){
				//处理事件
				Socket &sock = **iter;
				if(iter == rw_socks.end()){
					//到达末尾 跳出
					break;
				}
				else if(FD_ISSET(sock.fd,&rw_set)){
					//发送读入事件
					ev.type = Event::READ;
					ev.sock = &sock;
					push_event(ev);
					-- ret;
				}
				++ iter;
			}
		}
		//重新填写timeout 检查是否被修改
		check_timeval(timeout_tmp,timeout);
		//写出数据 事件
		ret = select(rw_count,nullptr,&rw_set,nullptr,tptr);
		if(ret < 0){
			//错误
			goto has_error;
		}
		else if(ret != 0){
			iter = rw_socks.begin();
			while(ret != 0){
				Socket &sock = **iter;
				if(iter == rw_socks.end()){
					break;
				}
				else if(FD_ISSET(sock.fd,&rw_set)){
					//找到被设置的Socket
					ev.type = Event::WRITE;
					ev.sock = &sock;
					push_event(ev);
					-- ret;
				}
				++ iter;
			}
		}
		check_timeval(timeout_tmp,timeout);
		LOG("Loop!\n");
	}
}
void Xv::push_event(Event &ev){
	pthread_mutex_lock(&mutex);
	//加入
	events_queue.push(ev);
	pthread_mutex_unlock(&mutex);
}
void Xv::push_xv_event(Event &ev){
	pthread_mutex_lock(&mutex);
	//加入XV事件
	xv_queue.push(ev);
	pthread_mutex_unlock(&mutex);
}
void Xv::add_socket(Socket *sock){
	//加入可以读写的Socket
	Event ev;
	ev.type = Event::XV_ADD_SOCK;
	ev.sock = sock;
	push_xv_event(ev);
}
void Xv::add_accept_socket(Socket *sock){
	//加入接收Socket
	Event ev;
	ev.type = Event::XV_ADD_ACCEPT_SOCK;
	ev.sock = sock;
	push_xv_event(ev);
}
void Xv::set_timeout(time_t s,long us){
	//设置超时事件
	timeout.tv_sec = s;
	timeout.tv_usec = us;
}
int Xv::poll_event(Event *ev){
	//拉取出事件
	if(events_queue.empty()){
		//没有事件
		return 0;
	}
	pthread_mutex_lock(&mutex);
	*ev = events_queue.front();
	events_queue.pop();
	pthread_mutex_unlock(&mutex);
	return true;
}
int Xv::wait_event(Event *ev){
	while(!poll_event(ev)){
		sleep(timeout.tv_sec);
		usleep(timeout.tv_usec);
	}
	return 1;
}
