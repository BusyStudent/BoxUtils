#include <curl/curl.h>
#include <curl/curlver.h>
#include <mutex>
//狐池锁
#include "common/def.hpp"
#include "net/easy.hpp"
#include "net/share.hpp"
#include "net/factory.hpp"

#include "backtrace.hpp"

namespace Box{
namespace Net{
	struct Share::Mutexs{
		//一个共享接口锁的结构体
		std::mutex m_cookie;//对于cookie的锁定
		std::mutex m_dns;//DNS锁定
		std::mutex m_ssl_session;//SSL会话锁定
		std::mutex m_connect;//连接池子锁定
		#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR >= 61
		// 7.61.0后的功能
		std::mutex m_psl;//公共后缀列表锁定
		#endif
		std::mutex m_share;//share加入曲柄时候的锁
	};
	//生成和释放曲柄
	//锁定函数
	typedef void (*ShareLockFn)(CURL*,curl_lock_data,curl_lock_access,void *);
	typedef void (*ShareUnLockFn)(CURL*,curl_lock_data,void *);

	Share::Share(){

	ShareLockFn lock_fn = [](CURL*,curl_lock_data data,curl_lock_access,void *mutexs) -> void{
		Share::Mutexs * m = static_cast<Share::Mutexs*>(mutexs);
		switch(data){
			case CURL_LOCK_DATA_COOKIE:
				//锁定cookie
				m->m_cookie.lock();
				break;
			case CURL_LOCK_DATA_DNS:
				//锁dns
				m->m_dns.lock();
				break;
			case CURL_LOCK_DATA_SSL_SESSION:
				//锁SSL
				m->m_ssl_session.lock();
				break;
			case CURL_LOCK_DATA_CONNECT:
				//锁连接
				m->m_connect.lock();
				break;
			#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR >= 61
			case CURL_LOCK_DATA_PSL:
				//锁PSL
				m->m_psl.lock();
				break;
			#endif
			case CURL_LOCK_DATA_SHARE:
				//share加入曲柄时候的锁
				m->m_share.lock();
				break;
			default:{
				abort();
			}
			//出错
		}
	};
	//解锁函数
	ShareUnLockFn unlock_fn = [](CURL *,curl_lock_data data,void *mutexs){
		Share::Mutexs * m = static_cast<Share::Mutexs*>(mutexs);
		switch(data){
			case CURL_LOCK_DATA_COOKIE:
				//解锁定cookie
				m->m_cookie.unlock();
				break;
			case CURL_LOCK_DATA_DNS:
				//解锁dns
				m->m_dns.unlock();
				break;
			case CURL_LOCK_DATA_SSL_SESSION:
				//解锁SSL
				m->m_ssl_session.unlock();
				break;
			case CURL_LOCK_DATA_CONNECT:
				//解锁连接
				m->m_connect.unlock();
				break;
			#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR >= 61
			case CURL_LOCK_DATA_PSL:
				//解锁PSL
				m->m_psl.unlock();
				break;
			#endif
			case CURL_LOCK_DATA_SHARE:{
				//share加入曲柄时候的锁
				m->m_share.unlock();
				break;
			}
			default:{
				abort();
			}
			//出错
		}
	};
		handle = curl_share_init();
		mutexs = new Share::Mutexs();
		curl_share_setopt(handle,CURLSHOPT_USERDATA,mutexs);
		//把锁带入
		curl_share_setopt(handle,CURLSHOPT_LOCKFUNC,lock_fn);
		curl_share_setopt(handle,CURLSHOPT_UNLOCKFUNC,unlock_fn);
	}
	Share::~Share(){
		curl_share_cleanup(handle);
		delete mutexs;//删除锁
	}

	//得到曲柄
	void *Share::get_handle() const{
		return handle;
	}
	//设置
	void Share::set_share_cookie(){
		//共享cookie
		curl_share_setopt(handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_COOKIE);
	}
	void Share::set_share_connect(){
		//共享连接
		curl_share_setopt(handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_CONNECT);
	}
	void Share::set_share_dns(){
		//共享DNS
		curl_share_setopt(handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_DNS);
	}
	void Share::set_share_ssl_session(){
		//共享SSL会话ID
		curl_share_setopt(handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_SSL_SESSION);
	}
	void Share::set_share_psl(){
		//共享公共后缀名
		#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR >= 61
		curl_share_setopt(handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_PSL);
		#endif
	}
	void Share::set_share_all(){
		//分享上面所有的
		set_share_connect();
		set_share_cookie();
		set_share_dns();
		set_share_psl();
		set_share_ssl_session();
	}


	//加入曲柄
	void Share::add_handle(Easy &easy){
		curl_easy_setopt(easy.handle,CURLOPT_SHARE,this->handle);
	}
	//加入曲柄指针
	void Share::add_handle(Easy *easy){
		add_handle(*easy);
	}
	//加入包
	void Share::add_handle(EasyPackage &pak){
		add_handle(*(pak.easy_handle));
	}
	//加入包指针
	void Share::add_handle(EasyPackage *pak){
		add_handle(*(pak->easy_handle));
	}


	//移除
	void Share::remove_handle(Easy &easy){
		curl_easy_setopt(easy.handle,CURLOPT_SHARE,nullptr);
	}
	//移除曲柄指针
	void Share::remove_handle(Easy *easy){
		remove_handle(*easy);
	}
	//移除包
	void Share::remove_handle(EasyPackage &pak){
		remove_handle(*(pak.easy_handle));
	}
	//移除包指针
	void Share::remove_handle(EasyPackage *pak){
		remove_handle(*(pak->easy_handle));
	}
}
}
