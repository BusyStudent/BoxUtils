#ifndef _BOXNET_HEADERS_HPP_
#define _BOXNET_HEADERS_HPP_
#include <functional>
namespace Box{
	class Json;
	namespace Net{
		class Headers{
			//头的实现(里面的值可能重复)
			public:
				Headers();
				Headers(const Headers &);
				~Headers();
				//构造器和析构器
				Box::Json *json();//到Json
				void add_string(const char *str);//添加字符串
				void add(const char *key,const char *value);
				void update(const Headers &headers);//更新头
				bool remove(const char *key);//移除一个值
				bool has_key(const char *key);//是否有这键
				const char *get_value(const char *key);//得到值(没有异常)
				const char *operator[](const char *key);//有异常的
				void for_each(std::function <void(const char*,const char*)>);//遍历头
			private:
				void *slist;
			friend class Easy;
		};
		
	};
};
#endif