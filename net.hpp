#ifndef _BOXNET_HPP_
#define _BOXNET_HPP_
#include "net_easy.hpp"
#include "net_headers.hpp"
namespace Box{
	namespace Net{
		//初始化
		bool Init(bool init_all = true);
		void Quit();
		const char *Version();//版本
	};
};
#endif
