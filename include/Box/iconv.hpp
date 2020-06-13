#ifndef _BOX_ICONV_HPP_
#define _BOX_ICONV_HPP_
//转换
#include <string>
#include <string_view>
#include <exception>
namespace Box{
	class IconvError:public std::exception{
		//Iconv错误
		public:
			IconvError(int code);
			//打开编码失败
			~IconvError();
			int get(){
				return err_code;
			};
			const char *what() const noexcept;
		private:
			int err_code;//错误代码
	};
	class Iconv{
		//字符串转换器 iconv的封装
		public:
			enum class Flag{
				//转换flag
				NONE = 0,//没有
				IGNORE = 1,//忽略错误字符
				TRANSLIT = 2//转换字符
			};
			typedef IconvError Error;
			Iconv(const char *to,const char *from,Flag = Flag::NONE);
			Iconv(const Iconv &) = delete;
			~Iconv();
			void convert(const std::string_view &in,std::string &output);//转换字符串
			void reset();//重置
			std::string convert(const std::string_view &in);//转换字符串
		private:
			void *cd;
	};
	namespace Charset{
		//字符集
		extern const char *GBK;
		extern const char *UTF8;
	};
};
#endif
