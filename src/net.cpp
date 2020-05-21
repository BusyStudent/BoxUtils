#include <curl/curl.h>
#include <utility>
#include "net.hpp"
namespace Box{
	bool Net::Init(bool init_all){
		//初始化
		long flags;
		if(init_all){
			flags = CURL_GLOBAL_ALL;
		}
		else{
			flags = CURL_GLOBAL_DEFAULT;
		}
		auto code = curl_global_init(flags);
		if(code == CURLE_OK){
			return true;
		}
		else{
			return false;
		}
	}
	void Net::Quit(){
		//退出
		curl_global_cleanup();
	}
	const char *Net::Version(){
		return curl_version();
	}
	//复制链表
	curl_slist* Net::SListDup(curl_slist *slist){
		curl_slist *new_list = nullptr;
		while(slist != nullptr){
			new_list = curl_slist_append(slist,slist->data);
		}
		return new_list;
	}
	std::string Net::Get(const std::string &url){
		//轻松的Get一下
		Easy easy;
		easy.set_url(url);
		std::string content;
		easy.set_ostream(content);
		return std::move(content);
	}
}