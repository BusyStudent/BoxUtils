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
				EasyException(ErrType type,const char*);
				const char *what() const throw();
				ErrType type();//得到类型
			private:
				ErrType _type;
				const char *_str;
		};
	};
};
#endif
