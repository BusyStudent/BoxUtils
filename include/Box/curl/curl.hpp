#if !defined(_BOX_CURL_HPP_)
#define _BOX_CURL_HPP_
#include "easy.hpp"
#include "factory.hpp"
#include "exception.hpp"
#include "headers.hpp"
#include "multi.hpp"
#include "share.hpp"
#include "../libc/attr.h"
namespace Box{
	namespace Curl{
		//初始化
		BOXAPI void Init(bool init_all = true);
		BOXAPI void Quit();
		BOXAPI const char *Version();//版本
		//发送Get请求 得到网页
		BOXAPI std::string Get(std::string_view URL);
	};
};
#endif // _BOX_CURL_HPP_
