#ifndef _BOXNET_FACTORY_HPP_
#define _BOXNET_FACTORY_HPP_
//工厂
#include <functional>
#include "net_easy.hpp"
#include "net_headers.hpp"
namespace Box{
	namespace Net{
		class EasyPackage{
			//工厂产出的对象
			public:
				EasyPackage(Easy *e,const Headers &h);
				EasyPackage(const EasyPackage&);
				~EasyPackage();
				inline Easy *operator ->() const{
					return easy_handle;
				}
				inline Easy &operator *() const{
					return *easy_handle;
				}
				inline const char *operator [](const char *key) const{
					return resp_headers[key];
				}
				inline Headers &headers(){
					return resp_headers;
				}
				void for_content(std::function <void(std::string&)>);//遍历内容
				std::string content();//得到内容
			private:
				Headers resp_headers;//响应头
				Headers _headers;//请求头
				Easy *easy_handle;
			friend class Multi;
			friend class Share;
			friend class RequestID;
		};
		class EasyFactory{
			//Easy对象工厂
			public:
				void update_headers();//更新proxey
				Headers &headers();//得到headers
				void add_header(const char *key,const char *value);
				EasyPackage create(const char *url);//直接从模板里克隆一个 在栈上
				EasyPackage *allocate(const char *url);//在堆上
				
				inline Easy *operator ->(){
					return &_easy;
				}
				inline Easy &operator *(){
					return _easy;
				}
			private:
				Headers _headers;//内置的头 要复制的
				Easy _easy;//内置的handle
		};
	};
};
#endif
