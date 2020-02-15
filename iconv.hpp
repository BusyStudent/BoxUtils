#ifndef _BOXUTILS_ICONV_HPP_
#define _BOXUTILS_ICONV_HPP_
//转换
#include <iconv.h>
namespace BoxUtils{
	class Iconv{
		public:
			Iconv(const char *to,const char *from);
			~Iconv();
			char *convert(char *str);
			static char *ConvertString(iconv_t cd,char *str);//核心的转换部分
			static char *Utf8ToGbk(char *str);
			static char *GbkToUtf8(char *str);
		private:
			iconv_t cd;//描述符
	};
};
#endif
