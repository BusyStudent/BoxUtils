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
		//超出边界
		public:
			IndexError(int index);
			const char *what()const throw();
			int index;
			std::string reason;
	};
	class KeyError:public std::exception{
		//没找到键
		public:
			KeyError(const char *key);
			KeyError(const KeyError&);
			~KeyError();
			const char *what()const noexcept;
			const std::string &value() const noexcept{
				return key;
			}
		private:
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
			NullPtrException();
			NullPtrException(const NullPtrException &);
			~NullPtrException();
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
			FileNotFoundError(const FileNotFoundError &err);
			~FileNotFoundError();
		private:
			//文件名字
			std::string fname;
	};
	class JsonParseError:public Exception{
		public:
			JsonParseError(const char *msg);
			JsonParseError(const JsonParseError&);
			~JsonParseError();
			const char *what() const throw();
		private:
			std::string msg;
	};
	void Panic(const char *fmt,...);//退出
	[[noreturn]] void throwNullPtrException();
};
#endif
