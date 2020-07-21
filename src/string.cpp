#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <vector>
#include <locale>
#include <stdexcept>
#include "base64.hpp"
#include "string.hpp"
namespace Box{
	String String::trim() const{
		const_iterator i_begin = begin();
		const_iterator i_end = --end();//得到末尾迭代器的前一个
		//得到开始和结束
		while(*i_begin == ' ' and i_begin != end()){
			//移到没有空格的
			++ i_begin;
		}
		while(*i_end == ' ' and i_end != begin()){
			-- i_end;
		}
		return String(i_begin,i_end);
	}
	//替换所有
	String String::replace_all(const std::string &old,const std::string &ne) const{
		String self(*this);
		//拷贝一下
		std::string & str = self.get();
		size_type old_len = old.length();
		size_type new_len = ne.length();
		size_type first = 0;
		size_type pos = str.find(old,first);//位置
		//替换所有
		while(pos != npos){
			str.replace(pos,old_len,ne);
			first = pos + new_len;
			pos = str.find(old,first);
		}
		return self;
	}
};
