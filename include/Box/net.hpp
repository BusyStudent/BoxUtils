#ifndef _BOXNET_HPP_
#define _BOXNET_HPP_
#include <string_view>
#include "net/easy.hpp"
#include "net/factory.hpp"
#include "net/headers.hpp"
#include "net/socket.hpp"
#include "net/share.hpp"
#include "net/poll.hpp"
struct curl_slist;
namespace Box{
	namespace Net{
		//初始化
		bool Init(bool init_all = true);
		void Quit();
		const char *Version();//版本
		//复制链表
		curl_slist *SListDup(curl_slist *slist);
		//发送Get请求 得到网页
		std::string Get(std::string_view URL);
	};
};
#endif
