#ifndef _BOXNET_HEADERS_HPP_
#define _BOXNET_HEADERS_HPP_
#include <functional>
#include <initializer_list>
struct curl_slist;
namespace Box{
	class Json;
	namespace Net{
		class Headers{
			//头的实现(里面的值可能重复)
			public:
				Headers();
				Headers(const std::initializer_list<const char*> &);
				Headers(const std::initializer_list<const std::initializer_list<const char*>>&);
				/*从字符串列表构造 如第一个是
					{
						"UserAgent:Hello"
					}
					第二个是
					{
						{
							userAgent,"Hello"
						}
					}
				*/
				Headers(Headers &&);//移动构造器
				Headers(const Headers &);
				~Headers();
				//构造器和析构器
				Box::Json json();//到Json
				void add_string(const char *str);//添加字符串
				void add(const char *key,const char *value);
				void update(const Headers &headers);//更新头
				bool remove(const char *key);//移除一个值
				bool has_key(const char *key) const;//是否有这键
				const char *get_value(const char *key) const;//得到值(没有异常)
				const char *operator[](const char *key) const;//有异常的
				void for_each(const std::function <void(const char*,const char*)>&) const;//遍历头
			private:
				curl_slist *slist;
			friend class Easy;
			friend struct MimePart;
		};
	};
};
#endif
