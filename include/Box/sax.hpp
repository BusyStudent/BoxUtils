#ifndef _BOX_SAX_HPP_
#define _BOX_SAX_HPP_
//SAX解析器
#include <cstddef>
#include <cstdarg>
struct _xmlParserCtxt;
struct _xmlSAXHandler;
namespace Box{
	namespace SAX{
		class Handler{
			//回调控制器
			public:
				Handler();
				Handler(const Handler &) = delete;
				~Handler();
				virtual void start_elem(const char *name,const char **attrs);//开始的时候调用
				virtual void end_elem(const char *name);//元素结束时候调用
				virtual void error();//遇到错误的时候调用
				//XML的字符串函数
				static int strcmp(const char *s1,const char *s2) noexcept;
			private:
				_xmlSAXHandler *handler;
			friend class XMLParser;
			friend class HTMLParser;
		};
		class XMLParser{
			public:
				XMLParser(Handler &hander);
				XMLParser(const XMLParser &) = delete;
				~XMLParser();
				void parse_string(const char *str);//解析字符串
				void parse_chunk(const void *chunk,int size);//解析块
				void done();//解析完成
				void reset() noexcept;//重置
			private:
				_xmlParserCtxt *ctxt;
		};
		class HTMLParser{
			//HTML解析器
			public:
				HTMLParser(Handler &handler);
				HTMLParser(const HTMLParser &) = delete;
				~HTMLParser();
				void parse_string(const char *str);//解析字符串
				void parse_chunk(const void *chunk,int size);//解析块
				void done();//解析完成
				void reset() noexcept;//重置
			private:
				_xmlParserCtxt *ctxt;
		};
	}
}
#endif
