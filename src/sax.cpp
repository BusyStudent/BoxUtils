#include <libxml/SAX2.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
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
	struct Functions{
		//所有回调的集合
		SAXHandler::StartElemFunc start_elem;
		inline static Functions &From(SAXHandler &sax){
			return *static_cast<Functions*>(sax.get_handler()->_private);
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
	void SAXHandler::start_elem(StartElemFunc fn){
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
			Functions::From(*handler).start_elem((const char*)name,handler->attrs);
		};
		Functions::From(*this).start_elem = std::move(fn);
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
};
};