#ifndef _BOXNET_EXCEPTION_HPP_
#define _BOXNET_EXCEPTION_HPP_
//异常
#include <string>
#include <exception>
namespace Box{
	namespace Net{
		class Exception:public std::exception{
			//基本异常
			public:
				const char *what() const throw();
				std::string reason;
		};
		class TimeoutError:public Exception{
			//超时
			public:
				TimeoutError(std::string &hostname,int timeout);
				int timeout;
		};
		class ConnectionError:public Exception{
			//连接错误
			public:
				ConnectionError(const char *reason,int code);
				int code;
		};
		class UnkownHostname:public Exception{
			//未知的域名
			public:
				UnkownHostname(std::string &hostname);
				std::string hostname;
		};
	};
};
#endif
