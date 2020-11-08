#ifndef _BOX_SAX_HPP_
#define _BOX_SAX_HPP_
//SAX解析器
#include <vector>
#include <functional>
#include <string_view>
struct _xmlParserCtxt;
struct _xmlSAXHandler;
static_assert(__cplusplus >= 2017L,"这个头文件需要C++17支持!!!");
namespace Box{
	namespace LXml{
		class Html;
		class Xml;
		template<typename T> class Parser;
		class SAXHandler{
			//SAX解析器
			public:
				//回调的定义
				typedef std::vector<std::string_view> AttrList;//元素列表
				typedef std::function<void()> StartFunc;//解析器开始的回调
				typedef std::function<void()> EndFunc;//解析器结束的回调
				typedef std::function<void(const std::string_view &,const AttrList&)> StartElemFunc;//开始元素
				typedef std::function<void(const std::string_view &)> EndElemFunc;//结束元素
				typedef std::function<void(const std::string_view &,const std::string_view&,const std::string_view&)> AttrFunc;//定义元素的回调
				//第一个是elem 的名字 第二个是元素的名字 第三个是元素的数值
			public:
				SAXHandler();
				SAXHandler(const SAXHandler &) = delete;
				SAXHandler(SAXHandler &&);
				~SAXHandler();
				void start_elem(const StartElemFunc &func);//开始元素
				void end_elem(const EndElemFunc &func);//元素结束时候调用
				void def_attr(const AttrFunc &func);//定义元素的回调
				//开始和结束的回调
				void start(const StartFunc &func);
				void end(const EndFunc &func);
			private:
				struct Functions;//函数的集合 内部实现
				_xmlSAXHandler *handler;
				AttrList attrs;//属性的地方
				
			friend class Parser<Xml>;
			friend class Parser<Html>;
		};
		//XmlParser
		template<>
		class Parser<Xml>{
			//Xml解析器
			public:
				Parser(SAXHandler &);
				Parser(const Parser &) = delete;
				Parser(Parser &&);
				~Parser();
				//解析一个块
				void add_string(const std::string_view &str);//添加字符串
				void parse_chunk(const void *mem,int size,int terminate = false);
				void reset();//重置
				void done();//结束解析
			private:
				_xmlParserCtxt *ctxt;
		};
		//HtmlParser
		template<>
		class Parser<Html>{
			public:
				Parser(SAXHandler &);
				~Parser();
			private:
				_xmlParserCtxt *ctxt;
		};
		typedef Parser<Xml> XmlParser;
		typedef Parser<Html> HtmlParser;
	};
}
#endif
