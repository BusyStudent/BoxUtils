#ifndef _BOXUTILS_NET_HPP_
#define _BOXUTILS_NET_HPP_
#include <string>
#include <vector>
#include <curl/curl.h>
namespace BoxUtils{
	namespace Net{
		class Response{
			public:
				Response(CURL*,void*);
			private:
				std::string buffer;
				void *_session;
				CURL *handle;
			friend class Session;
		};
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
				const char *find_value(const char *key);//查询值
				const char *operator [](const char *key);
				curl_slist *get_slist();
			private:
				curl_slist *slist;//单链表
		};
		class Session{
			public:
				Session();
				~Session();
				Response *Get();
				Response *Post();
				void clear_handles();//清空Handles
				void add_handle(CURL *handle);
				CURL *allocate_handle();//得到handle
				//属性
				Headers headers;
			private:
				std::vector <CURL*> handles_pool;
		};
		extern Session *_session;//会话
		void Init(long flag = CURL_GLOBAL_ALL);
		void Quit();
		Response *Get();
		Response *Post();
	};
};
#endif
