#include <libxml/SAX2.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <string>
#include "xml.hpp"
#include "sax.hpp"
using namespace Box;
//SAX解析器的实现
SAX::Handler::Handler(){
	//初始化一个handler
	handler = (xmlSAXHandlerPtr)malloc(sizeof(xmlSAXHandler));//初始化一个Handler
	memset(handler,0,sizeof(xmlSAXHandler));//清空
	handler->initialized = XML_SAX2_MAGIC;
	
	handler->startElement = [](void *ctxt,const xmlChar *name,const xmlChar **attrs) -> void{
		//开始的元素时候调用
		Handler *handler = (Handler*)ctxt;
		return handler->start_elem((const char*)name,(const char**)attrs);
	};
	handler->endElement = [](void *ctxt,const xmlChar *name){
		//在元素结束时候调用
		Handler *handler = (Handler*)ctxt;
		handler->end_elem((const char*)name);
	};
}
SAX::Handler::~Handler(){
	//释放掉
	free(handler);
}
//默认的callback
void SAX::Handler::start_elem(const char *,const char **){
	
}
void SAX::Handler::end_elem(const char *){
	
}
void SAX::Handler::error(){
	
}
//字符串函数
int SAX::Handler::strcmp(const char *s1,const char *s2) noexcept{
	return xmlStrcmp(BAD_CAST s1,BAD_CAST s2);
}
//XMLParser
SAX::XMLParser::XMLParser(Handler &handler){
	ctxt = xmlCreatePushParserCtxt(handler.handler,&handler,nullptr,0,nullptr);
}
SAX::XMLParser::~XMLParser(){
	xmlFreeParserCtxt(ctxt);
}
//解析字符串
void SAX::XMLParser::parse_string(const char *str){
	this->parse_chunk(str,strlen(str) * sizeof(char));
}
//解析块
void SAX::XMLParser::parse_chunk(const void *chunk,int size){
	if(xmlParseChunk(ctxt,(const char*)chunk,size,0) != 0){
		//失败
		throw std::runtime_error(xmlCtxtGetLastError(ctxt)->message);
	}
}
void SAX::XMLParser::done(){
	//结束解析
	if(xmlParseChunk(ctxt,nullptr,0,1) != 0){
		//失败
		throw std::runtime_error(xmlCtxtGetLastError(ctxt)->message);
	}
}
void SAX::XMLParser::reset() noexcept{
	xmlCtxtReset(ctxt);
}
//HTMLParser
SAX::HTMLParser::HTMLParser(Handler &handler){
	//创建handler 默认编码UTF8
	ctxt = htmlCreatePushParserCtxt(handler.handler,&handler,nullptr,0,nullptr,XML_CHAR_ENCODING_UTF8);
}
SAX::HTMLParser::~HTMLParser(){
	htmlFreeParserCtxt(ctxt);
}
//解析字符串
void SAX::HTMLParser::parse_string(const char *str){
	this->parse_chunk(str,strlen(str) * sizeof(char));
}
//解析块
void SAX::HTMLParser::parse_chunk(const void *chunk,int size){
	if(htmlParseChunk(ctxt,(const char*)chunk,size,0) != 0){
		//失败
		throw std::runtime_error(xmlCtxtGetLastError(ctxt)->message);
	}
}
void SAX::HTMLParser::done(){
	//结束解析
	if(htmlParseChunk(ctxt,nullptr,0,1) != 0){
		//失败
		throw std::runtime_error(xmlCtxtGetLastError(ctxt)->message);
	}
}
void SAX::HTMLParser::reset() noexcept{
	htmlCtxtReset(ctxt);
}
