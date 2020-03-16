#ifndef _BOXNET_EASY_HPP_
#define _BOXNET_EASY_HPP_
#include <string>
namespace Box{
	namespace Net{
		//简单网络接口
		enum class Method{
			POST,
			GET,
			PUT
		};
		class Headers;
		typedef size_t (*EasyCallBack)(char*,size_t,size_t,void*);
		class Easy{
			public:
				Easy(void *);//通过Handle直接生成
				Easy();
				Easy(const Easy&);
				~Easy();
				void perform();//执行
				void set_headers();//重置会原有的请求头
				void set_headers(const Headers &);//替换原有的请求头
				void set_method(Method method);//设置方法
				void set_url(const char *url);//设置URL
				void set_proxy(const char *proxy);//设置代理
				void set_timeout(long timeout);//设置超时
				void set_useragent(const char *str);//设置User-Agent
				void set_referer(const char *str);//设置Referer
				void set_ostream(std::string &str);//设置输出内容的字符流
				void set_ostream(FILE *f);//设置输出内容的字符流
				void set_oheaders(Headers &headers);//设置输出的头
				void set_followlocation();//自动更寻重定向
				void reset();//重置
				
				//CURL的回调
				void set_write_cb(EasyCallBack cb,void *param = nullptr);//设置写出的回调
				void set_header_cb(EasyCallBack cb,void *param = nullptr);//设置头的回调
				
				void *get_handle();//得到handle
				long status_code();
				bool ok();//请求是否成功
				
				Easy *clone();//复制自己
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
