#ifndef _BOX_EXCEPTION_HPP_
#define _BOX_EXCEPTION_HPP_
//一下异常
#include <exception>
#include <stdexcept>
#include <string>
namespace Box{
	class Json;
	typedef std::invalid_argument InvalidArgument;
	typedef std::exception Exception;
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
	class OSError:public std::exception{
		//操作系统的错误
		public:
			OSError(int code,const char *msg = nullptr,const char *extra = nullptr);
			const char *what() const throw();
			int code;//代码
			std::string msg;//信息
			std::string extra;//附加信息
			static std::string Format(int code,const char *msg,const char *extra);//格式化信息
		private:
			std::string what_msg;//what输出信息
	};
	class FileNotFoundError:public OSError{
		//文件没有找到的错误
		public:
			FileNotFoundError(int code,const char *filename);
			std::string filename;
	};
	class JsonParseError:public Exception{
		public:
			JsonParseError(const char *msg);
			const char *what() const throw();
		private:
			std::string msg;
	};
	void Panic(const char *fmt,...);//退出
};
#endif
