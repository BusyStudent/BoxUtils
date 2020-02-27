#include <string>
#include <cstring>
#include <climits>
#include "base64.hpp"
using namespace BoxUtils;
const char *Base64::EncodeTable="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char Base64::DecodeTable[]=
{
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
		-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
		-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};
std::string Base64::Encode(const char *str){
	return Base64::Encode(str,strlen(str));
}
std::string Base64::Encode(const char *data,size_t datalen){
	std::string buf;
	auto cptr = data;
	const char *& table = Base64::EncodeTable;
	while(datalen > 2){
		//编码
		buf += table [ cptr[0] >> 2];//第一个字符向右移2位
		buf += table [ ((cptr[0] << 4) | (cptr[1] >> 4)) & 0x3f];
		//第一个字符高6位去掉然后左移四位 第二个字符右移四位
		buf += table [ ((cptr[1] << 2) | (cptr[2] >> 6)) & 0x3f];
		//第二个字符去掉高4位并左移两位 第三个字符右移6位并去掉高6位
		buf += table [ cptr[2] & 0x3f];
		//第三字符去掉高2位
		cptr += 3;
		datalen -= 3;
	}
	switch(datalen){
		case 2:{
			//只有两个字符
			buf += table [ cptr[0] >> 2];
			buf += table [ ((cptr[0] << 4) | (cptr[1] >> 4)) & 0x3f];
			buf += table [ (cptr[1] & 0x0f) << 2 ];
			buf += '=';
			break;
		}
		case 1:{
			buf += table [ cptr[0] >> 2];
			buf += table [ (cptr[1] << 4) & 0x3f];
			
			buf += '=';
			buf += '=';
			//加两个等号
			break;
		}
	}
	return buf;
}
std::string Base64::Decode(const char *str){
	return Base64::Decode(str,strlen(str));
}
std::string Base64::Decode(const char *data,size_t datalen){
	//这个函数的出处是https://www.cnblogs.com/lrxing/p/5535601.html
	const char *DecodeTable = Base64::DecodeTable;
	size_t bin = 0,i=0;
	size_t &length = datalen;
	const char base64_pad = '=';
	std::string _decode_result;
	const char *current = data;
	char ch;
	while( (ch = *current++) != '\0' && length-- > 0 )
	{
		if (ch == base64_pad) { // 当前一个字符是“=”号
			/*
			先说明一个概念：在解码时，4个字符为一组进行一轮字符匹配。
			两个条件：
				1、如果某一轮匹配的第二个是“=”且第三个字符不是“=”，说明这个带解析字符串不合法，直接返回空
				2、如果当前“=”不是第二个字符，且后面的字符只包含空白符，则说明这个这个条件合法，可以继续。
			*/
			if (*current != '=' && (i % 4) == 1) {
				return NULL;
			}
			continue;
		}
		ch = DecodeTable[ch];
		//这个很重要，用来过滤所有不合法的字符
		if (ch < 0 ) { /* a space or some other separator character, we simply skip over */
			continue;
		}
		switch(i % 4)
		{
			case 0:
				bin = ch << 2;
				break;
			case 1:
				bin |= ch >> 4;
				_decode_result += bin;
				bin = ( ch & 0x0f ) << 4;
				break;
			case 2:
				bin |= ch >> 2;
				_decode_result += bin;
				bin = ( ch & 0x03 ) << 6;
				break;
			case 3:
				bin |= ch;
				_decode_result += bin;
				break;
		}
		i++;
	}
	return _decode_result;
}
