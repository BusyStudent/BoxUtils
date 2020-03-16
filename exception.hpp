#ifndef _BOX_EXCEPTION_HPP_
#define _BOX_EXCEPTION_HPP_
//一下异常
#include <stdexcept>
namespace Box{
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
	class NullPtrException:public std::exception{
		//空指针错误
		public:
			const char *what() const throw();
	};
};
#endif
