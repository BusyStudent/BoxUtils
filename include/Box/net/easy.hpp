#ifndef _BOXNET_EASY_HPP_
#define _BOXNET_EASY_HPP_
#include <string>
#include <iosfwd>
#include <cstddef>
struct curl_mime_s;
struct curl_mimepart_s;
namespace Box{
	namespace Net{
		//简单网络接口
		enum class Method{
			POST,
			GET,
			PUT
		};
		class Mime;//表单
		class Headers;
		typedef size_t (*EasyCallBack)(char*,size_t,size_t,void*);
		class Easy{
			public:
				Easy(void *);//通过Handle直接生成
				Easy();
				Easy(Easy &&easy);
				Easy(const Easy&);
				~Easy();
				void perform();//执行
				void enable_cookie();//启动Cookie引擎
				void add_cookie(const char *cookie);//添加cookie 在引擎内
				void set_cookie(const char *cookie);//设置cookie 在cookie引擎外部
				void set_headers();//重置会原有的请求头
				void set_headers(const Headers &);//替换原有的请求头
				void set_method(Method method);//设置方法
				void set_url(const char *url);//设置URL
				void set_url(const std::string &url);//设置URL
				void set_proxy(const char *proxy);//设置代理
				void set_timeout(long timeout);//设置超时
				void set_useragent(const char *str);//设置User-Agent
				void set_referer(const char *str);//设置Referer
				void set_ostream(std::string &str);//设置输出内容的字符流
				void set_ostream(std::ostream &stream);//设置标准库的输出流
				void set_ostream(FILE *f);//设置输出内容的字符流
				void set_oheaders(Headers &headers);//设置输出的头
				void set_post(const void *data,long size,bool copy = true);
				//设置为Post 第一个是数据 第二个是数据大小 第三个是是否拷贝一份
				void set_post(const std::string &str);//设置Post的字符串数据
				void set_post(const Mime &mime);//设置post的表单
				void set_followlocation();//自动更寻重定向
				void reset();//重置

				void clear_cookie();//清空cookie
				void reset_cookie();//清空回话cookie
				
				//CURL的回调
				void set_write_cb(EasyCallBack cb,void *param = nullptr);//设置写出的回调
				void set_header_cb(EasyCallBack cb,void *param = nullptr);//设置头的回调
				
				void throw_for_status();//当状态代码不为200时候抛出异常

				void *get_handle() const;//得到handle
				std::string url() const;//得到URL
				long status_code() const;
				bool ok()const;//请求是否成功
				
				Easy *clone()const;//复制自己
				//编码URL和解码
				std::string escape_url(const char *url) const;
				std::string unescape_url(const char *irl) const;

				//得到自己引用从CURL Handle
				static Easy &GetRefFrom(void *handle);
			private:
				//回调函数
				static size_t WriteToString(char*,size_t,size_t,void*);//写出内容到字符串
				static size_t WriteToFILE(char*,size_t,size_t,void*);//写出内容到字符流
				static size_t WriteToHeaders(char*,size_t,size_t,void*);//写到头里面
				void *handle;
				void *multi_userdata;//给multi使用的数据指针
			friend class Multi;
			friend class Share;
			friend class Mime;
		};
		struct MimePart{
			//表单的一部分
			void set_headers();//清空
			void set_headers(const Headers &,bool copy = true);//设置头部的数值 默认复制一下
			void set_encoder(const char *encoder);//设置
			void set_name(const char *name);//设置名字
			void set_data(const void *data,size_t datasize);//设置数据从内存中
			void set_type(const char *type);//设置类型
			void set_filedata(const char *filename);//设置数据从文件中
			curl_mimepart_s *part;
		};
		class Mime{
			public:
				Mime(const Easy &easy);
				Mime(const Mime&) = delete;
				~Mime();
				MimePart addpart();//添加一个部分
			private:
				curl_mime_s *mime;//表单数据
			friend class Easy;
		};
	};
};

#endif
