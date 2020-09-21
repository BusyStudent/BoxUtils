#ifndef _BOX_STRING_HPP_
#define _BOX_STRING_HPP_
#include <initializer_list>
#if __cplusplus >= 201703L
	#include <string_view>
#endif
#include <string>
#include <string_view>
#include <vector>
#include "libc/attr.h"
namespace Box{
	//String工具
	class BOXAPI String:public std::string{
		public:
			//转发参数构造一下
			template<class ...Args>
			String(Args &&...args):
				std::string::basic_string(std::forward<Args>(args)...){
				
			}
			//移除前面和末尾空格
			String trim() const noexcept;
			//转换到标题样式
			String title() const noexcept;
			//切片
			String slice(long begin = 0,long end = 0) const noexcept;
			//分割 第一个参数是容器 第二个是元素
			template<template<class> class T = std::vector,class Elem = String>
			T<Elem> split(std::string_view delim) const noexcept{
				T<Elem> vec;
				split_to<T,Elem>(vec,delim);
				return vec;
			};
			//分割 不过这个需要输出容器在参数中
			template<template<class> class T = std::vector,class Elem = String>
			const String& split_to(T<Elem> &vec,std::string_view delim) const noexcept{
				size_type pos = 0;
				size_type next;//下一个
				while(pos < length()){
					next = find_first_of(delim,pos);
					if(next == npos){
						//没找到 加入剩下的
						//原来写法是emplace_back(substr(pos,length() - pos));
						vec.emplace_back(&(self()[pos]),length() - pos);
						break;
					}
					else{
						vec.emplace_back(&(self()[pos]),next - pos);
					}
					pos = next + 1;
				}
				return *this;
			};
			template<template<class> class T = std::vector,class Elem = String>
			String& split_to(T<Elem> &vec,std::string_view delim) noexcept{
				size_type pos = 0;
				size_type next;//下一个
				while(pos < length()){
					next = find_first_of(delim,pos);
					if(next == npos){
						//没找到 加入剩下的
						vec.emplace_back(&(self()[pos]),length() - pos);
						break;
					}
					else{
						vec.emplace_back(&(self()[pos]),next - pos);
					}
					pos = next + 1;
				}
				return *this;
			}
			//指向自身引用
			const String &self() const noexcept{
				return *this;
			}
			String &self() noexcept{
				return *this;
			}
	};
};
#endif
