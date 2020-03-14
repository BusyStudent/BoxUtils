#ifndef _BOXNET_EASY_HPP_
#define _BOXNET_EASY_HPP_
#include <string>
namespace Box{
	namespace Net{
		//简单网络接口
		namespace Method{
			enum T{
				POST,
				GET,
				FETCH
			};
		};
		class Headers;
		class Easy{
			public:
				Easy();
				Easy(const Easy&);
				~Easy();
				void perform();//执行
				void set_headers();//重置会原有的请求头
				void set_headers(const Headers &);//替换原有的请求头
				void set_method(Method::T method);//设置方法
				void set_url(const char *url);//设置URL
				void set_proxy(const char *proxy);//设置代理
				void set_useragent(const char *str);
				void set_ostream(std::string &str);//设置输出内容的字符流
				void set_ostream(FILE *f);//设置输出内容的字符流
				void set_oheaders(Headers &headers);//设置输出的头
				void reset();//重置
				void *get_handle();//得到handle
			private:
				//回调函数
				static size_t WriteToString(char*,size_t,size_t,void*);//写出内容到字符串
				static size_t WriteToFILE(char*,size_t,size_t,void*);//写出内容到字符流
				static size_t WriteToHeaders(char*,size_t,size_t,void*);//写到头里面
				void *handle;
		};
	};
};

#endif
