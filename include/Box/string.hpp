#ifndef _BOX_STRING_HPP_
#define _BOX_STRING_HPP_
#include <initializer_list>
#if __cplusplus >= 201703L
	#include <string_view>
#endif
#include <cstdlib>
#include <cstring>
#include <string>
namespace Box{
	class String{
		public:
			//转换一下
			template<class ...Args>
			String(const Args&...args):str(args...){}
			//右值引用
			String(std::string &&s):str(s){}
		public:
			typedef std::string::size_type     size_type;
			
			typedef std::string::iterator       iterator;
			typedef std::string::const_iterator const_iterator;

			typedef std::string::reference reference;
			typedef std::string::const_reference const_reference;
			//一些std::string本来就有的东西
			const static size_type npos = std::string::npos;
			//迭代器
			iterator begin() noexcept{
				return str.begin();
			}
			iterator end() noexcept{
				return str.end();
			}
			const_iterator begin() const noexcept{
				return str.begin();
			}
			const_iterator end() const noexcept{
				return str.end();
			}
			//数组index
			const_reference operator [](size_type index) const noexcept{
				return str[index];
			}
			reference operator [](size_type index) noexcept{
				return str[index];
			}

			const char *c_str() const noexcept{
				return str.c_str();
			}
		public:
			//自己加入的东西
			#if __cplusplus >= 201703L
			//添加string_view以后的东西
			typedef std::string_view _View;
			operator std::string_view() const noexcept{
				return std::string_view(str);
			}
			#else
			typedef std::string _View;
			#endif
			template<class String,class T>
			void split(const String &str,T &out) const noexcept{
				return impl_split(str,out);
			}
			template<class Ret,class String>
			//懒人切割
			Ret split(const String &str){
				Ret ret;
				impl_split(str,ret);
				return ret;
			}
			String replace_all(const std::string &,const std::string &n) const;//移除所有
			String trim() const;//首位移除空格
			operator std::string() const noexcept{
				return str;
			}
			//得到内部字符串引用
			std::string &get() noexcept{
				return str;
			}
			const std::string &get() const noexcept{
				return str;
			}
		private:
			//切割字符串的实现
			template<class Out>
			void impl_split(const _View &delim,Out &out) const noexcept{
				size_type len = delim.length();
				size_type first = 0;
				size_type pos = str.find(delim,first);
				while(pos != npos){
					out.push_back(str.substr(first,pos));
					first = pos + len;
					pos = str.find(delim,first);
				}
			};
		private:
			std::string str;
	};
};
#endif
