#ifndef _BOX_LOGGER_HPP_
#define _BOX_LOGGER_HPP_
//记录器
#include <cinttypes>
#include <string>
namespace Box{
	class Logger{
		//记录器
		public:
			struct FakeStream{
			//记录器的流
				inline FakeStream &operator <<(const std::string &str){
					logger->write(str.c_str());
					return *this;
				};
				inline FakeStream &operator <<(const char *text){
					//写字符串
					logger->write(text);
					return *this;
				};
				inline FakeStream &operator <<(char ch){
					logger->putc(ch);
					return *this;
				};
				template <class T>
				inline FakeStream &operator <<(const T &t){
					//转换字符串写出
					logger->write(std::to_string(t).c_str());
					return *this;
				};
				Logger *logger;//记录器指针
			};
			//默认初始化在终端里面
			Logger();
			//输出在文件里面
			~Logger();
			//设置输出
			void set_output();//设置输出在标准输出
			void set_output(const std::string &output);
			//设置网络输出
			void set_output(const std::string &ip,uint16_t port);
			//C风格的输出函数 自动换行(除了write putc)
			void write(const char * text);//写字符串
			void putc(int ch);//写字符
			void error(const char *fmt,...);
			void warn(const char *fmt,...);
			void log(const char *fmt,...);
			//C++风格的输出函数
			FakeStream &error();
			FakeStream &warn();
			FakeStream &log();
			void flush();//刷新
			
		private:
			//具体实现
			void *impl;
	};
};
#endif
