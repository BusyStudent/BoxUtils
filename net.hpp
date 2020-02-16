#ifndef _BOXUTILS_NET_HPP_
#define _BOXUTILS_NET_HPP_
#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
namespace BoxUtils{
	namespace Net{
		typedef size_t (*CURLCallBack)(void*,size_t,size_t,void*);
		size_t WriteContentToMem(void*,size_t,size_t,void*);//写出内容
		size_t WriteHeadersToMem(void*,size_t,size_t,void*);//写出应答头
		class Session;
		class Headers{
			public:
				Headers();
				Headers(const Headers &);
				~Headers();
				Headers *copy();//复制一个
				void append(const char *key,const char *val);
				void add_string(const char *str);//添加字符串
				void update(const Headers &);
				void clear();//清空
				void print();//打印一下
				void parse_string(const char *str);//解析字符串
				const char *find_value(const char *key);//查询值
				const char *operator [](const char *key);
				curl_slist *get_slist();
				Json *to_json();//转换到Json
				void load_json(Json &json);//加载Json到头部
			private:
				curl_slist *slist;//单链表
			friend class Session;
		};
		class Response{
			public:
				~Response();
				Headers headers;
				std::string headers_string;//字符串头部
				std::string content;//内容
				void build_headers();//构建头部
				long status_code;
				Json *json();//解析到Json
			private:
				Session *_session = nullptr;
				CURL *handle = nullptr;
			friend class Session;
		};
		class Session{
			public:
				Session(int default_handles = 2);//初始存放handle
				~Session();
				Response *get(const char *url,Headers *headers=nullptr,long timeout=0);
				Response *post();
				void clear_handles();//清空Handles
				void add_handle(CURL *handle);
				CURL *allocate_handle();//得到handle
				//属性
				Headers headers;
			private:
				CURLCallBack write_content_func;//默认写出内容的函数
				CURLCallBack write_headers_func;//默认写出请求头
				std::vector <CURL*> handles_pool;
		};
		extern Session *session;//会话
		void Init(long flag = CURL_GLOBAL_ALL);
		void Quit();
		Response *Get(const char *url,Headers *headers=nullptr,long timeout=0);
		Response *Post();
	};
};
#endif
