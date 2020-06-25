#include <string>
#include <cstring>
#include <climits>
#include <cinttypes>
#include <stdexcept>
#include "base64.hpp"
namespace Box{
	static inline uint8_t QueryTable(uint8_t ch) noexcept{
		//查找表
		return strchr(Base64::Table,ch) - Base64::Table;
	}
	const char Base64::Table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string Base64::Encode(const std::string &str){
		//编码字符串
		auto s = (const uint8_t*)(str.c_str());
		return Base64::Encode(s,strlen((char*)s));
	}
	std::string Base64::Encode(const uint8_t *str){
		return Base64::Encode(str,strlen((char*)str));
	}
	std::string Base64::Encode(const uint8_t *str,size_t datalen){
		std::string buf;
		Base64::Encode(str,datalen,buf);
		return buf;
	}
	void Base64::Encode(const uint8_t *data,size_t datalen,std::string &buf){
		auto cptr = data;
		auto table = Base64::Table;
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
	}
	std::string Base64::Decode(const std::string &str){
		//解码字符串
		auto s = (const uint8_t*)(str.c_str());
		return Base64::Decode(s,strlen((char*)s));
	}
	std::string Base64::Decode(const uint8_t *str){
		return Base64::Decode(str,strlen((char*)str));
	}
	std::string Base64::Decode(const uint8_t *str,size_t datalen){
		std::string buf;
		Base64::Decode(str,datalen,buf);
		return buf;
	}
	void Base64::Decode(const uint8_t *data,size_t datalen,std::string &buf){
		if((datalen % 4) != 0){
			//字符串长度必须是4的倍数
			throw std::invalid_argument("Bad Base64 String len");
		}
		auto cptr = data;//临时指针
		size_t sub = 0;//减去的量
		
		if(data[datalen - 1] == '='){
			sub ++;
		}
		if(data[datalen - 2] == '='){
			sub++;
		}
		//这里去掉后面的==号
		datalen -= sub;
		
		uint8_t array[4];//字符数组
		while(datalen > 3){
			array[0] = QueryTable(cptr[0]);
			array[1] = QueryTable(cptr[1]);
			array[2] = QueryTable(cptr[2]);
			array[3] = QueryTable(cptr[3]);
			//将着4个字符依次转换
			buf += ((array[0] << 2) | (array[1] >> 4));
			//第一个字符左移动两位 第二个字节右移4位 得到第一个 
			buf += ((array[1] << 4) | (array[2] >> 2));
			//第二个字符左边移4 第三个右移2位
			buf += ((array[2] << 6) | array[3]);
			//第三个字符左边移动位
			cptr += 4;//移动临时指针
			datalen -= 4; 
		}
		switch(datalen){
			//还剩几个字符
			case 0:{
				//没了
				break;
			}
			case 2:{
				//还有2个字符
				array[0] = QueryTable(cptr[0]);
				array[1] = QueryTable(cptr[1]);
				buf += ((array[0] << 2) | (array[1] >> 4));
				break;
			}
			case 3:{
				
				array[0] = QueryTable(cptr[0]);
				array[1] = QueryTable(cptr[1]);
				array[2] = QueryTable(cptr[2]);
				buf += ((array[0] << 2) | (array[1] >> 4));
				//第一个字符左移动两位 第二个字节右移4位 得到第一个 
				buf += ((array[1] << 4) | (array[2] >> 2));
				break;
			}
			default:{
				//不可能
				abort();
			}
		}
	}
}
