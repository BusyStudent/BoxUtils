#ifndef _BOX_STRING_HPP_
#define _BOX_STRING_HPP_
#include <initializer_list>
#include <string_view>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
static_assert(__cplusplus >= 2017L,"这个头文件要C++17");
namespace Box{
	class StringHelper{
		//帮助处理字符串的类
		public:
			using vector = std::vector<std::string>;
			using Vector = std::vector<std::string>;
			//Bool来标记是否要拷贝字符串
			StringHelper(const char *str):str(str),owned(false){};
			//持有一个引用
			StringHelper(const char *str,bool):str(str),owned(true){};
			//持有一个字符串
			//同理
			StringHelper(const std::string &str):StringHelper(str.c_str()){};
			StringHelper(const std::string &str,bool i):StringHelper(str.c_str(),i){};

			StringHelper(const std::string_view &str):StringHelper(str.data()){};
			StringHelper(const std::string_view &str,bool i):StringHelper(str.data(),i){};
			StringHelper(const StringHelper &) = delete;
			//不能拷贝
			StringHelper(StringHelper &&helper){
				owned = helper.owned;
				str = helper.str;
				helper.owned = false;
			}
			~StringHelper();
			vector split(const std::string_view &delim);//切割
			StringHelper slice();//切片
			StringHelper trim();//剔除在首尾
			//编码和解码
			StringHelper decode(const std::string_view &encoding);
			StringHelper encode(const std::string_view &decoding);
			//格式化
			static StringHelper format(const std::string_view &fmt,...);
			static StringHelper format(const std::string_view &fmt,va_list);
			//得到C的字符串
			const char *c_str() const noexcept{
				return str;
			}
			size_t length() const noexcept{
				return strlen(str);
			}
			operator std::string() const noexcept{
				return std::string(str);
			};
			operator std::string_view() const noexcept{
				return std::string_view(str);
			}
		private:
			const char *str;//字符串
			bool owned;//是否有所有权
	};
	namespace String{
		//兼容之前的代码
		typedef std::vector<std::string> Vector;
	};
};
#endif
