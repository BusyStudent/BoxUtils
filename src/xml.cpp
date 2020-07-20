#include <libxml/xmlstring.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <utility>
#include <cstring>
#include <cassert>
#include <ostream>
#include "libc/atexit.h"
#include "xml.hpp"
#include "xpath.hpp"
#include "exception.hpp"
namespace Box{
namespace LXml{
	inline const char *CastString(const xmlChar *str){
		return (const char*)str;
	}
	XmlHolder::~XmlHolder(){
		if(not ref){
			//如果不是引用
			xmlFreeDoc(xml);
		}
	}
	NodeHolder::~NodeHolder(){
		if(not ref){
			xmlFreeNode(node);
		}
	}
	inline XmlHolder  *CreateXmlHodler(xmlDocPtr xml,bool ref){
		//创建一个XmlHolder
		return new XmlHolder{
			.xml = xml,
			.ref = ref
		};
	}
	inline NodeHolder *CreateNodeHodler(xmlNodePtr node,bool ref){
		//创建一个NodeHolder
		return new NodeHolder{
			.node = node,
			.ref = ref
		};
	}
	//XML解析错误异常
	Error::Error(xmlErrorPtr ptr){
		error = ptr;
	}
	Error::~Error(){}
	const char *Error::what() const throw(){
		//得到错误信息
		if(error != nullptr){
			return error->message;
		}
		return "Unknwon";
	}
	Xml::Xml(const char *version)
		:holder(CreateXmlHodler(xmlNewDoc(BAD_CAST(version)),false)){
		//创建一个Xml
	}
	Xml::Xml(xmlDocPtr xml,bool ref)
		:holder(CreateXmlHodler(xml,ref)){
		//从xmlDocPtr构建
	}
	Xml::Xml(const Xml &xml)
		:holder(xml.holder){
		//拷贝函数
	}
	Xml::Xml(Xml &&xml)
		:holder(xml.holder){
		//移动函数
	}
	Xml::~Xml(){
		//销毁
	}
	Xml Xml::ParseString(const char *str){
		xmlDocPtr xml = xmlParseDoc(BAD_CAST(str));
		if(xml == nullptr){
			//失败
			throw Error(xmlGetLastError());
		}
		return Xml(xml);
	}
	//解析字符串
	Xml Xml::ParseString(const std::string &str){
		return ParseString(str.c_str());
	}
	Xml Xml::LoadFile(const char *filename){
		//加载文件
		xmlDocPtr xml = xmlParseFile(filename);
		if(xml == nullptr){
			throw Error(xmlGetLastError());
		}
		return Xml(xml);
	}
	//得到信息
	bool Xml::is_ref() const{
		return holder->ref;
	}
	std::string Xml::version() const{
		//得到版本
		return CastString(holder->xml->version);
	}
	std::string Xml::encoding() const{
		//得到编码方式
		return CastString(holder->xml->encoding);
	}
	//转换为字符串
	std::string Xml::to_string() const{
		xmlChar *mem = nullptr;
		int len;
		xmlDocDumpFormatMemoryEnc(holder->xml,&mem,&len,DefaultEncoding,1);
		std::string str(CastString(mem),len);
		//到字符串
		xmlFree(mem);
		return std::move(str);
	}
	//保存到文件
	void Xml::savefile(const char *filename,int fmt,const char *enc){
		if(xmlSaveFormatFileEnc(filename,holder->xml,enc,fmt) != 0){
			throw Error(xmlGetLastError());
		}
	}
	//保存到文件流
	void Xml::dump(FILE *fstream){
		if(xmlDocDump(fstream,holder->xml) != 0){
			throw Error(xmlGetLastError());
		}
	}
	//位置
	Node Xml::root() const{
		//得到根节点
		xmlNodePtr root = xmlDocGetRootElement(holder->xml);
		if(root == nullptr){
			throw NullPtrException();
		}
		//返回引用
		return Node(root,true);
	}
	//设置根节点
	void Xml::set_root(Node &&node){
		xmlNodePtr old_node = nullptr;//原来节点
		if(not node.holder->ref){
			//如果不是引用
			node.holder->ref = true;
			old_node = xmlDocSetRootElement(holder->xml,node.holder->node);
			return ;
		}
		else{
			//复制一份 再加入
			old_node = xmlDocSetRootElement(holder->xml,xmlCopyNode(node.holder->node,1));
		}
		if(old_node != nullptr){
			xmlFreeNode(old_node);
		}
	}
	void Xml::set_root(Node &node){
		set_root(std::move(node));
	}
	//XPath查找
	XPath::Object Xml::xpath(const char *exp){
		XPath::Context ctxt(*this);
		//创建context
		return ctxt.eval(exp);
	}
	XPath::Object Xml::xpath(const std::string &exp){
		return xpath(exp.c_str());
	}
	//构建函数
	void Init(){
		xmlInitGlobals();
		libc::atexit_once(LXml::Quit);
	}
	void Quit(){
		xmlCleanupParser();
		xmlCleanupGlobals();
	}
	//释放和申请内存
	void  Free(void *mem){
		xmlFree(mem);
	}
	void *Malloc(size_t byte){
		return xmlMalloc(byte);
	}
	//输出到流
	std::ostream &operator<<(std::ostream &ostr,const Xml &xml){
		xmlChar *mem = nullptr;
		int len;
		xmlDocDumpFormatMemoryEnc(xml.holder->xml,&mem,&len,DefaultEncoding,1);
		ostr.write(CastString(mem),len);
		//到字符串
		xmlFree(mem);
		return ostr;
	}
	//常量
	const char *DefaultVersion = "1.0";
	const char *DefaultEncoding = "utf8";
};
};
//Node的实现
namespace Box{
namespace LXml{
	Node::Node(xmlNodePtr node,bool ref)
		:holder(CreateNodeHodler(node,ref)){
		//从指针构建
	}
	//复制和移动函数
	Node::Node(const Node &node)
		:holder(node.holder){

	}
	Node::Node(Node &&node)
		:holder(node.holder){
		
	}
	Node::~Node(){
		
	}
	//复制
	Node Node::copy() const{
		return Node(xmlCopyNode(holder->node,1));		
	}
	bool Node::is_ref() const{
		return holder->ref;
	}
	bool Node::is_text() const{
		return xmlNodeIsText(holder->node);
	}
	
	//得到其他节点的引用
	Node Node::next() const{
		if(holder->node->next == nullptr){
			throw NullPtrException();
		}
		return Node(holder->node->next,true);
	}
	Node Node::prev() const{
		if(holder->node->prev == nullptr){
			throw NullPtrException();
		}
		return Node(holder->node->prev,true);
	}
	Node Node::parent() const{
		if(holder->node->parent == nullptr){
			throw NullPtrException();
		}
		return Node(holder->node->parent,true);
	}
	Node Node::child() const{
		if(holder->node->children == nullptr){
			throw NullPtrException();
		}
		return Node(holder->node->children,true);
	}

	//提取出文本
	std::string Node::text() const{
		xmlChar *mem = XML_GET_CONTENT(holder->node);
		if(mem == nullptr){
			//得到内容失败
			throw NullPtrException();
		}
		return std::string(CastString(mem));
	}
	//提取出名字
	std::string Node::name() const{
		if(holder->node->name == nullptr){
			throw NullPtrException();
		}
		return std::string(CastString(holder->node->name));
	}
	//到字符串
	std::string Node::to_string() const{
		xmlBufferPtr buf = xmlBufferCreate();
		//创建缓冲区
		xmlNodeDump(buf,holder->node->doc,holder->node,1,1);
		std::string str(CastString(buf->content));
		xmlBufferFree(buf);
		return str
	}
	//提取出属性
	std::string Node::operator[](const char *name) const{
		xmlAttrPtr attr = xmlHasProp(holder->node,BAD_CAST name);
		if(attr == nullptr){
			throw KeyError(name);
		}
		return CastString(XML_GET_CONTENT(attr->children));
	}
	std::string Node::operator[](const std::string &name) const{
		return operator[](name.c_str());
	}
	//设置文本
	void Node::set_text(const std::string &text){
		xmlNodeSetContent(holder->node,BAD_CAST(text.c_str()));
	}
	void Node::set_text(const char *text){
		xmlNodeSetContent(holder->node,BAD_CAST(text));
	}
	//设置名字
	void Node::set_name(const std::string &name){
		xmlNodeSetName(holder->node,BAD_CAST(name.c_str()));
	}
	void Node::set_name(const char *name){
		xmlNodeSetName(holder->node,BAD_CAST(name));
	}
	//添加孩子
	void Node::add_child(Node &&node){
		if(not node.holder->ref){
			//不是引用
			node.holder->ref = true;//改为引用
			//直接加入
			xmlAddChild(holder->node,node.holder->node);
			return;
		}
		else{
			//拷贝一下
			xmlAddChild(holder->node,xmlCopyNode(node.holder->node,1));
		}
	}
	void Node::add_child(Node &node){
		add_child(std::move(node));
	}
	//添加attr
	void Node::add_attr(const char *attr,const char *value){
		xmlNewProp(holder->node,BAD_CAST(attr),BAD_CAST(value));
	}
	//是否有属性
	bool Node::has_attr(const char *attr) const{
		return xmlHasProp(holder->node,BAD_CAST(attr)) != nullptr;
	}
	bool Node::has_attr(const std::string &attr) const{
		return has_attr(attr.c_str());
	}
	XPath::Object Node::xpath(const char *str){
		Xml xml = Xml::ParseString(to_string());
		//转换为xml
		return xml.xpath(str);
	}
	XPath::Object Node::xpath(const std::string &str){
		return xpath(str.c_str());
	}
	//输出到ostream
	std::ostream &operator<<(std::ostream &ostr,const Node &node){
		xmlBufferPtr buf = xmlBufferCreate();
		//创建缓冲区
		xmlNodeDump(buf,node.holder->node->doc,node.holder->node,1,1);
		ostr.write((CastString(buf->content)),buf->use);
		xmlBufferFree(buf);
		return ostr;
	}
};
};
//Html
namespace Box{
namespace LXml{
	//Html
	Html Html::ParseString(const char *str){
		htmlDocPtr html = htmlReadDoc(BAD_CAST str,nullptr,DefaultEncoding,
			XML_PARSE_NOERROR | XML_PARSE_NONET | XML_PARSE_RECOVER | XML_PARSE_NOWARNING);
		//解析字符串
		if(html == nullptr){
			throw Error(xmlGetLastError());
		}
		return Html(html,false);
	}
	Html Html::ParseString(const std::string &str){
		return ParseString(str.c_str());
	}
	std::string Html::to_string() const{
		//到字符串
		xmlChar *mem;
		int size;
		htmlDocDumpMemoryFormat(holder->xml,&mem,&size,1);
		std::string str(CastString(mem),size);
		xmlFree(mem);
		return str
	}
	//保存
	void Html::savefile(const char *filename,int fmt,const char *enc){
		if(htmlSaveFileFormat(filename,holder->xml,enc,fmt) != 0){
			//失败
			throw Error(xmlGetLastError());
		}
	}
	void Html::dump(FILE  *fstream){
		if(htmlDocDump(fstream,holder->xml) != 0){
			throw Error(xmlGetLastError());
		}
	}
	std::ostream &operator<<(std::ostream &ostr,const Html &html){
		xmlChar *mem;
		int size;
		htmlDocDumpMemoryFormat(html.holder->xml,&mem,&size,1);
		ostr.write(CastString(mem),size);
		xmlFree(mem);
		return ostr;
	}
};
};
