#include <cstring>
#include <string>
#include <vector>
#include "string.hpp"
using namespace Box;
String::Vector String::Split(
	const char *str,
	const char *subs){
	std::vector <std::string> vec;
	//切割字符串
	size_t subslen = strlen(subs);//子串的长度
	const char *begin = str;//开始字符串
	const char *end = nullptr;//找到的部分
	while(true){
		end = strstr(begin,subs);
		//查找一下
		if(end == nullptr){
			break;
		}
		else{
			std::string s(begin,end);
			//截取一下
			vec.push_back(s);
			begin = end;//移动到末尾
			begin += subslen;//跳过去
		}
	}
	return vec;
}
String::Vector String::Split(
	const char *str,
	const char *subs,
	int max){
	int cutted = 0;//初始切了0份
	std::vector <std::string> vec;
	//切割字符串
	size_t subslen = strlen(subs);//子串的长度
	const char *begin = str;//开始字符串
	const char *end = nullptr;//找到的部分
	while(true){
		end = strstr(begin,subs);
		//查找一下
		if(end == nullptr){
			break;
		}
		else{
			std::string s(begin,end);
			//截取一下
			vec.push_back(s);
			begin = end;//移动到末尾
			begin += subslen;//跳过去
			cutted ++;
			if(cutted == max){
				//达到最大数目
				break;
			}
		}
	}
	return vec;
}
