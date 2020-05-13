#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <vector>
#include <stdexcept>
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
			//把最后一片加入
			vec.push_back(std::string(begin));
			break;
		}
		else{
			//截取一下
			vec.push_back(std::string(begin,end));
			begin = end;//移动到末尾
			begin += subslen;//跳过去
		}
	}
	return vec;
}
String::Vector String::Split(
	const char *str,
	const char *subs,
	unsigned int max){
	unsigned int cutted = 0;//初始切了0份
	std::vector <std::string> vec;
	//切割字符串
	size_t subslen = strlen(subs);//子串的长度
	const char *begin = str;//开始字符串
	const char *end = nullptr;//找到的部分
	while(true){
		end = strstr(begin,subs);
		//查找一下
		if(end == nullptr){
			vec.push_back(std::string(begin));
			break;
		}
		else{
			//截取一下
			vec.push_back(std::string(begin,end));
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
//格式化字符串
std::string String::VFormat(const char *fmt,va_list &varg){
	FILE *tf = tmpfile();//申请临时文件
	if(tf == nullptr){
		//失败
		abort();
	}
	//写入数据
	vfprintf(tf,fmt,varg);
	
	auto len = ftell(tf);//得到字符串长度
	fseek(tf,0,SEEK_SET);//移动到文件开始处
	char buf[len + 1];
	memset(buf,'\0',sizeof(buf));
	fread(buf,len,1,tf);
	//读入数据
	std::string str(buf);
	fclose(tf);
	return str;
}
//格式化字符串
std::string String::Format(const char *fmt,...){
	va_list varg;
	va_start(varg,fmt);
	std::string str = String::VFormat(fmt,varg);
	va_end(varg);
	return str;
}
//切片
std::string String::Slice(const std::string &str,int begin,int end){
	size_t len = str.length();//得到长度
	std::string::size_type begin_pos;//开始位置
	std::string::size_type end_pos;//结束位置
	if(begin < 0){
		//是负数
		begin_pos = len + begin;
	}
	else{
		//正常的
		begin_pos = begin;
	}
	if(end < 0){
		end_pos = len + end;
	}
	else if(end == 0){
		//截取后面全部
		end_pos = len;
	}
	else{
		end_pos = end;
	}
	return str.substr(begin_pos,end_pos - begin_pos);
}
