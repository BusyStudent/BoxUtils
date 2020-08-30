#ifndef _BOXNET_EXCEPTION_HPP_
#define _BOXNET_EXCEPTION_HPP_
#include <exception>
#include <string>
#include "../libc/attr.h"
namespace Box{
	namespace Curl{
		class EasyException:public std::exception{
			public:
				EasyException(int curlcode);
				EasyException(const EasyException &);
				~EasyException();
				const char *what() const throw();
				inline int code(){
					return curlcode;
				};//得到代码
				//从curlcode抛出异常
				[[noreturn]] static void Throw(int curlcode);
			private:
				int curlcode;
		};
		class MultiException:public std::exception{
			//Multi发生了错误
			public:
				MultiException(int curlmcode);
				const char *what() const throw();
		};
		class HttpError:public std::exception{
			//Http的错误
			public:
				HttpError(int code);
				HttpError(const HttpError &);
				~HttpError();
				inline int status_code(){
					//得到状态代码
					return code;
				};
				const char *what() const throw();
			private:
				int code;
				std::string msg;
		};
		BOXAPI [[noreturn]] void  throwEasyException(int curlcode);
	};
};
#endif
