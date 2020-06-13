#ifndef _BOX_BASE64_HPP_
#define _BOX_BASE64_HPP_
#include <string>
#include <cinttypes>
namespace Box{
	namespace Base64{
		//Base64的表
		extern const char Table[];
		std::string Decode(const std::string &str);
		std::string Decode(const uint8_t *str);
		std::string Decode(const uint8_t *data,size_t datalen);
		void Decode(const uint8_t *data,size_t datalen,std::string &buf);
		//编码
		std::string Encode(const std::string &str);
		std::string Encode(const uint8_t *data,size_t datalen);
		std::string Encode(const uint8_t *str);
		void Encode(const uint8_t *str,size_t datalen,std::string &buf);
	};
}
#endif
