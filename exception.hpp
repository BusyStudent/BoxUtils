#ifndef _BOXUTILS_EXCEPTION_HPP_
#define _BOXUTILS_EXCEPTION_HPP_
//一下异常
#include <stdexcept>
namespace BoxUtils{
	class IndexError:public std::exception{
		public:
			IndexError(int index);
			const char *what()const throw();
			int index;
			std::string reason;
	};
	class KeyError:public std::exception{
		public:
			KeyError(const char *key);
			const char *what()const throw();
			std::string key;
	};
	class TypeError:public std::exception{
		public:
			TypeError(const char *excepted,const char *gived);
			const char *what()const throw();
			std::string gived;
			std::string excepted;
			std::string reason;
	};
};
#endif
