#ifndef _BOXNET_HEADERS_HPP_
#define _BOXNET_HEADERS_HPP_
#include <functional>
#include <string_view>
#include <initializer_list>
struct curl_slist;
namespace Box{
	struct JsonRef;
	class Json;
	namespace Curl{
		class Headers{
			//头的实现(里面的值可能重复)
			public:
				explicit Headers(JsonRef);
				Headers():slist(nullptr){};
				Headers(std::initializer_list<std::string_view>);
				Headers(std::initializer_list<std::initializer_list<std::string_view>>);
				/*从字符串列表构造 如第一个是
					{
						"UserAgent:Hello"
					}
					第二个是
					{
						{
							"UserAgent","Hello"
						}
					}
				*/
				Headers(Headers &&);//移动构造器
				Headers(const Headers &);
				~Headers();
				//构造器和析构器
				Box::Json json();//到Json
				
				void add_string(std::string_view str);//添加字符串
				void add(std::string_view key,std::string_view value);
				void update(const Headers &headers);//更新头
				bool remove(std::string_view key);//移除一个值
				bool has_key(std::string_view key) const;//是否有这键
				void for_each(const std::function <void(const char*,const char*)>&) const;//遍历头
				std::string_view operator[](std::string_view key) const;//有异常的
				std::string_view get_value (std::string_view key) const;//得到值(没有异常) 失败返回空指针
			private:
				curl_slist *slist;
			friend class Easy;
			friend struct MimePart;
		};
	};
};
#endif
