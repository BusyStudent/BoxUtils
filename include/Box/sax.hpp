#ifndef _BOX_SAX_HPP_
#define _BOX_SAX_HPP_
//SAX解析器
#include <vector>
#include <functional>
#include <string_view>
struct _xmlParserCtxt;
struct _xmlSAXHandler;
static_assert(__cplusplus > 2017L,"这个头文件需要C++17支持");
namespace Box{
	namespace LXml{
		class Html;
		class Xml;
		template<typename T>
		class Parser;
		class SAXHandler{
			//SAX解析器
			public:
				//回调的定义
				typedef std::vector<std::string_view> AttrList;
				typedef std::function<void(const std::string_view &,const AttrList&)> StartElemFunc;
			public:
				SAXHandler();
				SAXHandler(const SAXHandler &) = delete;
				SAXHandler(SAXHandler &&);
				~SAXHandler();
				void start_elem(StartElemFunc func);
				
				inline _xmlSAXHandler *get_handler(){
					//得到内部句柄
					return handler;
				};
			private:
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
			private:
				_xmlParserCtxt *ctxt;
		};
		//HtmlParser
		template<>
		class Parser<Html>{

		};
		typedef Parser<Xml> XmlParser;
		typedef Parser<Html> HtmlParser;
	};
}
#endif
