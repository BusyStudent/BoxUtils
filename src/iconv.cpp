#include <iconv.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <string>
#include "iconv.hpp"
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