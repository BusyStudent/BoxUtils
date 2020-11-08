#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <string>
#include <vector>
#include <locale>
#include <stdexcept>
#include <Box/common/def.hpp>
#include <Box/string.hpp>
namespace Box{
	String String::trim() const noexcept{
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
	String String::title() const noexcept{
		String s;
		const_iterator iter = begin();//记录当前位置
		const_iterator last = begin();//最开始的位置
		while(true){
			//跳过空格
			while(*iter == ' '){
				++iter;
				if(iter == end()){
					s.append(last,iter);
					return s;
				}
			}
			//放置前面所有内容
			if(iter != last){
				s.append(last,iter);
				last = iter;//保存内容开始
			}
			while(*iter != ' '){
				//得到内容结束
				++iter;
				if(iter == end()){
					//到达末尾
					s += toupper(*last);
					s.append(last + 1,iter);
					return s;
				}
			}
			s += toupper(*last);
			s.append(last + 1,iter);
			last = iter;
		}
	}
	String String::slice(long begin,long end) const noexcept{
		if(begin == 0 and end == 0){
			//复制一下
			return self();
		}
		if(end < 0){
			//反向迭代
			end = length() + end + 1;
		}
		if(begin < 0){
			//反向迭代
			begin = length() + begin + 1;
		}
		if(end == 0){
			return substr(begin);
		}
		return substr(begin,end);
	}
};
