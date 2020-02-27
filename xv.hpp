#ifndef _BOXUTILS_XV_HPP
#define _BOXUTILS_XV_HPP
//消息处理异步IO
#include <queue>
#include <vector>
#include <pthread.h>
#include <csignal>
#include "sem.hpp"
#include "socket.hpp"
namespace BoxUtils{
	class Xv{
		//一个会话
		public:
			struct Event{
				enum {
					WRITE,//写
					READ,//读入
					ACCEPT,//连接
					ERROR,//错误
					XV_ERROR, //XV出现错误
					XV_ADD_SOCK,//添加Socket
					XV_ADD_ACCEPT_SOCK,
					XV_DELETE_SOCK,//删除Socket
					XV_DELETE_ACCEPT_SOCK,
					XV_REMOVE_ACCEPT_SOCK,//移除Accept Socket
					XV_REMOVE_SOCK,//移除Socket
				}type;
				int errno_;
				Socket *sock;
			};
			int poll_event(Event *);//拉事件
			int wait_event(Event *);//等待事件
			void push_event(Event &);//压入事件
			Xv();
			~Xv();
			void add_socket(Socket *);
			void add_accept_socket(Socket *);//
			void run();//启动
			static void *ThreadMain(void *);//线程启动点
			void set_timeout(time_t s,long us);//设置超时时间
		private:
			void push_xv_event(Event &);//压入Xv事件
			struct timeval timeout;
			//每隔3秒 轮寻一次
			pthread_t thread;//线程
			pthread_mutex_t mutex;//锁
			fd_set rw_set;
			fd_set ap_set;//等待接入的集合
			std::vector <Socket*> ap_socks;
			std::vector <Socket*> rw_socks;
			std::queue <Event> events_queue;//事件队列
			std::queue <Event> xv_queue;//事件队列
	};
};
#endif
