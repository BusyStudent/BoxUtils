#ifndef _BOXUTILS_NET_EXCEPTION_HPP_
#define _BOXUTILS_NET_EXCEPTION_HPP_
//异常
#include <string>
#include <exception>
#include <curl/curl.h>
namespace BoxUtils{
	namespace Net{
		namespace Exception{
			class BaseException:public std::exception{
				public:
					//所以异常的基类
					BaseException(const char *reason);
					std::string reason;
					const char *what() const throw();
			};
			class NetError:public BaseException{
				//网络错误
				public:
					NetError(CURLcode code,CURL *handle,const char *url);
					~NetError();
					const char *what() const throw();
					std::string url;
					CURLcode code;
					CURL *handle;
				private:
					std::string format_string;//格式化过的字符串
			};
			class URLError:public NetError{
				public:
					URLError(CURLcode,CURL*,const char*);
			};
			class ConnectionError:public NetError{
				public:
					ConnectionError(CURLcode,CURL*,const char*);
			};
		};
		void ThrowExceptionByCURLcode(CURLcode code,CURL *handle,const char *url=nullptr);
	};
};
#endif
