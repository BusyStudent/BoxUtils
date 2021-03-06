#include <curl/curl.h>
#include <curl/curlver.h>
#include <iostream>
#include <functional>
#include "exception.hpp"
#include "net/exception.hpp"
#include "net/easy.hpp"
#include "net/multi.hpp"
#include "net/factory.hpp"
namespace Box{
namespace Net{
	/*
	static auto multi_select = [](void * handle,struct timeval *tv) -> int{
		fd_set wset,rset,eset;
		FD_ZERO(&wset);//清空集合
		FD_ZERO(&rset);//清空集合
		FD_ZERO(&eset);//清空集合
		int maxfd;//最大的fd
		curl_multi_fdset(handle,&wset,&rset,&eset,&maxfd);
		if(maxfd == -1){
			//没有fd
			return 0;
		}
		int ret = select(maxfd,&wset,&rset,&eset,tv);
		switch(ret){
			case -1:
				return -1;
				//Select错误
			case 0:
				//超时
				return 0;
			default:
				//有任务正在进行
				return 0;
		}
	};
	*/
	Multi::Multi(){
		//生成multi接口
		handle = curl_multi_init();
	}
	Multi::Multi(Multi &&m){
		handle = m.handle;
		m.handle = nullptr;
	}
	Multi::~Multi(){
		if(handle != nullptr){
			curl_multi_cleanup(handle);
		}
	}
	/*
	int Multi::select(struct timeval &tv){
		//不堵塞的select
		struct timeval temp = tv;
		return multi_select(handle,&temp);
	}
	int Multi::select(){
		//堵塞的select
		return multi_select(handle,nullptr);
	}
	*/
	void Multi::perform(int &running_handles){
		//调用一下running_handles
		auto code = curl_multi_perform(handle,&running_handles);
		switch(code){
			case CURLM_CALL_MULTI_PERFORM:
			case CURLM_OK:
				return;
			//这两个是正常的
			default:
				//错误
				return;
		}
	}
	int  Multi::perform(){
		int running;
		perform(running);
		return running;
	}
	void Multi::add_handle(Easy &easy,void *userdata){
		//添加曲柄
		//设置Easy的对象地址
		easy.multi_userdata = userdata;
		curl_multi_add_handle(handle,easy.handle);
	}
	void Multi::add_handle(Easy *easy,void *userdata){
		//添加曲柄指针
		return add_handle(*easy,userdata);
	}
	void Multi::add_handle(EasyPackage &pak,void *userdata){
		//添加曲柄包
		return add_handle(*(pak.easy_handle),userdata);
	}
	void Multi::add_handle(EasyPackage *pak,void *userdata){
		//添加曲柄包指针
		return add_handle(*(pak->easy_handle),userdata);
	}

	void Multi::remove_handle(Easy &easy){
		//移除曲柄
		curl_multi_remove_handle(handle,easy.handle);
	}
	void Multi::remove_handle(Easy *easy){
		//移除曲柄指针
		return remove_handle(*easy);
	}
	void Multi::remove_handle(EasyPackage &pak){
		//移除曲柄包
		return remove_handle(*(pak.easy_handle));
	}
	void Multi::remove_handle(EasyPackage *pak){
		return remove_handle(*(pak->easy_handle));
	}

	int Multi::wait(int timeout_ms){
		int numfds;
		#if LIBCURL_VERSION_MAJOR == 7 && LIBCURL_VERSION_MINOR >= 66
		CURLMcode code = curl_multi_poll(handle,nullptr,0,timeout_ms,&numfds);
		#else
		//某些低版本的CURL
		CURLMcode code = curl_multi_wait(handle,nullptr,0,timeout_ms,&numfds);
		#endif
		if(code != CURLM_OK){
			return -1;
		}
		return numfds;
	}
	int Multi::wait(const std::chrono::milliseconds &ms){
		return wait(ms.count());
	}
	bool Multi::get_msg(MultiMsg &msg,int &msg_in_queue){
		//得到信息 填充结构
		CURLMsg *cmsg;//CURL的信息
		do{
			cmsg = curl_multi_info_read(handle,&msg_in_queue);
			//得到信息
			if(cmsg == nullptr){
				//没有信息
				return false;
			}
			if(cmsg->msg != CURLMSG_DONE){
				//如果不是完成事件 忽略
				continue;
			}
			Easy &easy = Easy::GetRefFrom(cmsg->easy_handle);
			//找到曲柄的地址
			//和用户数据
			msg.multi = this;
			
			msg.easy = &easy;
			msg.userdata = easy.multi_userdata;
			
			msg.code = cmsg->data.result;
			return true;
		}
		while(true);
	}
	void Multi::set_max(long max){
		curl_multi_setopt(handle,CURLMOPT_MAXCONNECTS,max);
	}
	void Multi::for_msg(const std::function <void(const MultiMsg&)> &fn){
		MultiMsg msg;
		int msg_in_queue;
		while(get_msg(msg,msg_in_queue)){
			fn(msg);
		}
	}
	//一些信息的函数
	bool MultiMsg::ok() const noexcept{
		//是否正常 传输
		return this->code == CURLE_OK;
	}
	void MultiMsg::throw_for_error() const{
		if(code != CURLE_OK){
			//抛出异常
			throw EasyException(code);
		}
	}
	void MultiMsg::throw_for_status() const{
		return easy->throw_for_status();
	}
	const char* MultiMsg::strerr() const noexcept{
		return curl_easy_strerror((CURLcode)code);
	}
}
}
