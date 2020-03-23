#ifndef _BOX_STRING_HPP_
#define _BOX_STRING_HPP_
#include <string>
#include <vector>
namespace Box{
	namespace String{
		typedef std::vector <std::string> Vector;
		Vector Split(
			const char *str,
			const char *subs);//切割字符串
		Vector Split(
			const std::string &str,
			const std::string &subs);//切割字符串
		Vector Split(
			const char *str,
			const char *subs,
			unsigned int max);//切割字符串 有最大数目
		std::string Trim();//剔除开头末尾空格
	}
};
#endif
