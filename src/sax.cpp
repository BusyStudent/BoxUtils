#include <libxml/SAX2.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
#include <utility>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <string_view>
#include "xml.hpp"
#include "sax.hpp"
namespace Box{
namespace LXml{
	//SAXHandler
	struct SAXHandler::Functions{
		//所有回调的集合
		StartElemFunc start_elem;
		EndElemFunc   end_elem;//元素结束的时候调用
		AttrFunc      def_attr;//定义元素callback
		StartFunc     start;//开始
		EndFunc       end;//结束
		inline static Functions &From(SAXHandler *sax){
			return *static_cast<Functions*>(sax->handler->_private);
		};
	};
	SAXHandler::SAXHandler(){
		//初始化一个Handler
		handler = new xmlSAXHandler;
		//清空
		memset(handler,0,sizeof(xmlSAXHandler));
		handler->initialized = XML_SAX2_MAGIC;//初始化
		handler->_private = new Functions();
	}
	//删除
	SAXHandler::~SAXHandler(){
		delete static_cast<Functions*>(handler->_private);
		delete handler;
	}
	//移动
	SAXHandler::SAXHandler(SAXHandler &&sax){
		handler = sax.handler;
		sax.handler = nullptr;
	}
	void SAXHandler::start_elem(const StartElemFunc &fn){
		//在开始的节点
		handler->startElement = [](void *ctxt,const xmlChar *name,const xmlChar **attrs) -> void{
			//中间函数
			SAXHandler *handler = static_cast<SAXHandler *>(ctxt);
			//得到handler
			handler->attrs.clear();//清空属性
			if(attrs != nullptr){
				for(int i = 0;attrs[i] != nullptr; i ++){
					//加入属性
					handler->attrs.push_back((const char*)attrs[i]);
				}
			}
			Functions::From(handler).start_elem((const char*)name,handler->attrs);
		};
		//设置函数
		Functions::From(this).start_elem = fn;
	}
	//元素结束的回调
	void SAXHandler::end_elem(const EndElemFunc &fn){
		handler->endElement = [](void *ctxt,const xmlChar *name) -> void{
			//调用函数
			Functions::From(static_cast<SAXHandler*>(ctxt)).end_elem((const char*)name);
		};
		//设置函数
		Functions::From(this).end_elem = fn;
	}
	//定义元素回调
	void SAXHandler::def_attr(const AttrFunc &fn){
		handler->attributeDecl = 
			[](void *ctxt,const xmlChar *elem,const xmlChar *name,int,int,const xmlChar *value,xmlEnumerationPtr){
			Functions::From(static_cast<SAXHandler*>(ctxt)).def_attr(
				(const char*)elem,
				(const char*)name,
				(const char*)value
			);
		};
		//设置一下
		Functions::From(this).def_attr = fn;
	}
	//结束
	void SAXHandler::end(const EndFunc &fn){
		handler->endDocument = [](void *ctxt){
			Functions::From(static_cast<SAXHandler*>(ctxt)).end();
		};
		Functions::From(this).end = fn;
	}
	//开始
	void SAXHandler::start(const StartFunc &fn){
		handler->startDocument = [](void *ctxt){
			Functions::From(static_cast<SAXHandler*>(ctxt)).start();
		};
		Functions::From(this).start = fn;
	}
};
};
namespace Box{
namespace LXml{
	//XmlParser
	XmlParser::Parser(SAXHandler &handler){
		ctxt = xmlCreatePushParserCtxt(handler.handler,&handler,nullptr,0,nullptr);
		//创建一个推的解析器
	}
	XmlParser::~Parser(){
		if(ctxt != nullptr){
			xmlFreeParserCtxt(ctxt);
		}
	}
	void XmlParser::parse_chunk(const void *mem,int size,int terminate){
		//解析一个块
		if(xmlParseChunk(ctxt,static_cast<const char*>(mem),size,terminate) != 0){
			//失败
			throw Error(xmlCtxtGetLastError(ctxt));
		}
	}
	//添加字符串
	void XmlParser::add_string(const std::string_view &str){
		parse_chunk(str.data(),str.length());
	}
	void XmlParser::reset(){
		xmlCtxtReset(ctxt);
	}
	void XmlParser::done(){
		//完成解析
		parse_chunk(nullptr,0,1);
	}
};
};