#include <iconv.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include "iconv.hpp"
#define PLUS_BUFFER 100
using namespace BoxUtils;
Iconv::Iconv(const char *to,const char *from){
	cd = iconv_open(to,from);
}
Iconv::~Iconv(){
	iconv_close(cd);
}
char *Iconv::convert(char *str){
	return Iconv::ConvertString(cd,str);
}
char *Iconv::ConvertString(iconv_t cd,char *str){
	char *inbuf = str;//缓冲区
	size_t inlen = strlen(str);
	size_t outlen = inlen + 10;
	char *buf = (char*)malloc(outlen);
	//memset(buf,'\0',outlen);
	char *outbuf = buf;
	//缓冲区
	while(true){
		if(iconv(cd,&inbuf,&inlen,&outbuf,&outlen)==(size_t)-1){
			perror("Iconv");
			if(errno == E2BIG){
				//缓冲区用完了可以解决
				size_t converted = (outbuf - buf)/sizeof(char);
				//已经转换过的
				buf = (char*)realloc(buf,converted + PLUS_BUFFER + 1);
				outbuf = buf + converted;//跳转到那时的位置
				outlen += PLUS_BUFFER;
				//memset(outbuf,'\0',PLUS_BUFFER+1);
				//添加'\0'
				continue;
			}
			free(buf);
			return nullptr;
		}
		else{
			//转换完成
			break;
		}
	}
	*outbuf = '\0';
	return buf;
}
//内置转换
char *Iconv::GbkToUtf8(char *str){
	iconv_t cd = iconv_open("utf-8","gbk");
	char *ret = Iconv::ConvertString(cd,str);
	iconv_close(cd);
	return ret;
}
char *Iconv::Utf8ToGbk(char *str){
	iconv_t cd = iconv_open("gbk","utf-8");
	char *ret = Iconv::ConvertString(cd,str);
	iconv_close(cd);
	return ret;
}
