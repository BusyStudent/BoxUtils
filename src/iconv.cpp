#include <iconv.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <string>
#include "common/def.hpp"
#include "iconv.hpp"
/*
from https://stackoverflow.com/questions/7153935/how-to-convert-utf-8-stdstring-to-utf-16-stdwstring
std::wstring utf8_to_utf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch&0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch&0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch&0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

*/
namespace Box{
	//Iconv错误
	IconvError::IconvError(int code):err_code(code){}
	IconvError::~IconvError(){}
	const char *IconvError::what() const noexcept{
		//输出错误
		return strerror(err_code);
	}
	Iconv::Iconv(const char *to,const char *from,Flag flag){
		//打开描述符号
		switch (flag){
			case Flag::TRANSLIT:{
				//转换
				std::string tocode(to);
				tocode += "/TRANSLIT";
				cd =  iconv_open(to,from);
				break;
			}
			case Flag::IGNORE:{
				//忽略
				std::string tocode(to);
				tocode += "/IGNORE";
				cd =  iconv_open(to,from);
				break;
			}
			case Flag::NONE:{
				//没有flag
				cd =  iconv_open(to,from);
				break;
			}
		}
		if(cd == (iconv_t)-1){
			//失败
			throw Error(errno);
		}
		//启动实现
	}
	Iconv::~Iconv(){
		iconv_close(cd);
	}
	void Iconv::convert(const std::string_view &in,std::string &out){
		//复制一下
		std::vector<char> inbuf_vec(in.begin(),in.end());
		std::vector<char> outbuf_vec;
		size_t insize = inbuf_vec.size();
		size_t outsize = insize * 2;
		size_t output_byte = 0;//输出大小
		size_t tmp;//临时储存输出大小
		outbuf_vec.resize(insize * 2);
		char *inbuf = inbuf_vec.data();
		char *outbuf = outbuf_vec.data();
		while(insize){
			//转换全部
			tmp = outsize;
			if(iconv(cd,&inbuf,&insize,&outbuf,&tmp) == (size_t)-1){
				//出现错误
				switch(errno){
					case E2BIG:{
						//太小
						output_byte += outsize - tmp;//记录多少
						outsize = tmp;
						ptrdiff_t dif = outbuf - outbuf_vec.data();//算出指针差
						outsize += outbuf_vec.size() + 100;
						outbuf_vec.resize(outsize);
						outbuf = outbuf_vec.data() + dif;
						break;
					}
					default:{
						throw Error(errno);
					}
				}
			}
			else{
				//记录转换的多少
				output_byte += outsize - tmp;
				outsize = tmp;
			}
		}
		out.append(outbuf_vec.data(),output_byte);
	}
	std::string Iconv::convert(const std::string_view &str){
		//输出一下
		std::string output;
		convert(str,output);
		return output;
	}
	void Iconv::reset(){
		//重置
		iconv(cd,nullptr,nullptr,nullptr,nullptr);
	}
	//字符集
	namespace Charset{
		const char *GBK = "GBK";
		const char *UTF8 = "UTF8";
	};
};
