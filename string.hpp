#ifndef _BOX_STRING_HPP_
#define _BOX_STRING_HPP_
#include <string>
#include <vector>
namespace Box{
	typedef std::vector <std::string> StringVector;
	namespace Utils{
	StringVector SplitSting(
		const char *str,
		const char *subs);//切割字符串
	StringVector SplitSting(
		const char *str,
		const char *subs,
		int max);//切割字符串 有最大数目
	}
};
#endif
