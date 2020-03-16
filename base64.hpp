#ifndef _BOX_BASE64_HPP_
#define _BOX_BASE64_HPP_
#include <string>
namespace Box{
	namespace Base64{
		//Base64的表
		extern const char *EncodeTable;
		extern const char DecodeTable[];//解码表
		std::string Decode(const char *str);
		std::string Decode(const char *data,size_t datalen);
		std::string Encode(const char *data,size_t datalen);
		std::string Encode(const char *str);
	};
}
#endif
