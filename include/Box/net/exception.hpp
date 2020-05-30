#ifndef _BOXNET_EXCEPTION_HPP_
#define _BOXNET_EXCEPTION_HPP_
#include <exception>
namespace Box{
	namespace Net{
		enum class ErrType{
			//错误类型
			UNKNOWN = 0,
			TIMEOUT
		};
		class EasyException:public std::exception{
			public:
				EasyException(int curlcode);
				const char *what() const throw();
				inline ErrType type(){
					return _type;
				};//得到类型
				inline int code(){
					return _code;
				};//得到代码
				//从curlcode抛出异常
				static void ThrowFrom(int curlcode);
			private:
				int _code;
				ErrType _type;
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
				inline int status_code(){
					//得到状态代码
					return _code;
				};
				const char *what() const throw();
			private:
				int _code;
		};
	};
};
#endif
