#ifndef _BOXNET_TABLE_HPP_
#define _BOXNET_TABLE_HPP_
#include <vector>
#include <string>
namespace Box{
	namespace Net{
		struct _Item{
			std::string key;//键
			std::string value;//值
		};
		typedef std::vector <_Item> Table;//表
		class Proxies{
			//代理
		};
		class CookieJar{
			//Cookie
		};
		class Headers{
			public:
				void append(const char *key,const char *value);//加入数据
				void clear();//清空
				void update(Headers &);//更新
				Table &get_table();
				std::string *get(const char *key);//得到值 失败返回nullptr
				std::string &operator [](const char * key);
				std::string pop(const char *key);//弹出值来
				std::string to_string();//到字符串
			private:
				Table table;
		};
	};
};
#endif
