#include <libxml/xmlstring.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <utility>
#include <cstring>
#include <cassert>
#include <ostream>
#include "common/def.hpp"
#include "libc/atexit.h"
#include "lxml/xml.hpp"
#include "lxml/xpath.hpp"
#include "exception.hpp"
namespace Box{
namespace LXml{
	XmlHolder::~XmlHolder(){
		if(not ref){
			//如果不是引用
			xmlFreeDoc(xml);
		}
	}
	inline XmlHolder  *CreateXmlHodler(xmlDocPtr xml,bool ref){
		//创建一个XmlHolder
		return new XmlHolder{
			.xml = xml,
			.ref = ref
		};
	}
	//XML解析错误异常
	Error::Error(xmlErrorPtr ptr){
		if(ptr != nullptr){
			error = new xmlError;
			xmlCopyError(ptr,error);//复制错误
		}
		else{
			error = nullptr;
		}
	}
	Error::Error(const Error &err){
		if(err.error != nullptr){
			error = new xmlError;
			xmlCopyError(err.error,error);//复制错误
		}
		else{
			error = nullptr;
		}
	}
	Error::Error(Error &&err){
		error = err.error;
		err.error = nullptr;
	}
	Error::~Error(){
		delete error;
	}
	const char *Error::what() const throw(){
		//得到错误信息
		if(error != nullptr){
			return error->message;
		}
		return "Unknwon";
	}
	//throwError
	BOXAPI [[noreturn]] void throwError(){
		throw Error(xmlGetLastError());
	}
	BOXAPI [[noreturn]] void throwError(xmlErrorPtr ptr){
		throw Error(ptr);
	}
	//解析字符串
	Xml Xml::ParseString(std::string_view str){
		xmlDocPtr xml = xmlParseMemory(str.data(),str.length());
		if(xml == nullptr){
			//失败
			throwError();
		}
		return Xml(xml);
	}
	
	Xml Xml::LoadFile(std::string_view filename){
		//加载文件
		xmlDocPtr xml = xmlParseFile(filename.data());
		if(xml == nullptr){
			throwError();
		}
		return Xml(xml);
	}
	//得到信息
	std::string XmlRef::version() const{
		//得到版本
		return c_cast<char*>(value->version);
	}
	std::string XmlRef::encoding() const{
		//得到编码方式
		return c_cast<char*>(value->encoding);
	}
	//转换为字符串
	std::string XmlRef::to_string() const{
		xmlChar *mem = nullptr;
		int len;
		xmlDocDumpFormatMemoryEnc(value,&mem,&len,DefaultEncoding,1);
		std::string str(c_cast<char*>(mem),len);
		//到字符串
		xmlFree(mem);
		return str;
	}
	//保存到文件
	void XmlRef::savefile(const char *filename,int fmt,const char *enc){
		if(xmlSaveFormatFileEnc(filename,value,enc,fmt) != 0){
			throwError();
		}
	}
	//保存到文件流
	void XmlRef::dump(FILE *fstream){
		if(xmlDocDump(fstream,value) != 0){
			throwError();
		}
	}
	//位置
	NodeRef XmlRef::root() const{
		//得到根节点
		xmlNodePtr root = xmlDocGetRootElement(value);
		if(root == nullptr){
			throwNullPtrException();
		}
		//返回引用
		return NodeRef(root);
	}
	//设置根节点
	void XmlRef::set_root(Node &&node){
		xmlNodePtr old_node = nullptr;//原来节点
		if(*node.refcount == 1){
			//只有一个引用`
			old_node = xmlDocSetRootElement(value,node.value);
			delete node.refcount;
			node.refcount = nullptr;
		}
		else{
			//复制一份 再加入
			old_node = xmlDocSetRootElement(value,xmlCopyNode(node.value,1));
		}
		if(old_node != nullptr){
			xmlFreeNode(old_node);
		}
	}
	void XmlRef::set_root(NodeRef ref){
		xmlNodePtr ptr = xmlDocSetRootElement(value,xmlCopyNode(ref.value,1));
		if(ptr != nullptr){
			xmlFreeNode(ptr);
		}
	}
	//XPath查找
	XPath::Object XmlRef::xpath(std::string_view exp) const{
		XPath::Context ctxt(*this);
		//创建context
		return ctxt.eval(exp);
	}
	XPath::Object Xml::xpath(std::string_view exp) const{
		XPath::Context ctxt(*this);
		//创建context
		return ctxt.eval(exp);
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
	std::ostream &operator<<(std::ostream &ostr,XmlRef ref){
		xmlChar *mem = nullptr;
		int len;
		xmlDocDumpFormatMemoryEnc(ref.value,&mem,&len,DefaultEncoding,1);
		ostr.write(c_cast<char*>(mem),len);
		//到字符串
		xmlFree(mem);
		return ostr;
	}
	//常量
	const char *DefaultVersion = "1.0";
	const char *DefaultEncoding = "utf8";
	//Xml
	Xml::~Xml(){
		if(refcount != nullptr){
			(*refcount)--;
			if(refcount == 0){
				delete refcount;
				xmlFreeDoc(value);
			}
		}
	}
	Xml::Xml(const Xml &xml):XmlRef(xml),refcount(xml.refcount){
		//增加引用计数器
		if(refcount != nullptr){
			(*refcount) ++;
		}
		else{
			///计数器为空值 数值无效
			value = nullptr;
		}
	}
	//移动
	Xml::Xml(Xml &&xml):XmlRef(xml),refcount(xml.refcount){
		xml.refcount = nullptr;
	}
	//构造一个新的
	Xml::Xml():
		Xml(xmlNewDoc(BAD_CAST DefaultVersion)){
		
	}
	Xml::Xml(xmlDocPtr ptr):
		XmlRef(ptr){
		refcount = new int(1);
	}
	//深拷贝
	Xml::Xml(XmlRef ref):Xml(xmlCopyDoc(ref.value,1)){

	}
	Xml Xml::clone() const{
		return Xml(xmlCopyDoc(value,1));
	}
};
};
//Node的实现
namespace Box{
namespace LXml{
	bool NodeRef::is_text() const{
		return xmlNodeIsText(value);
	}
	
	//得到其他节点的引用
	NodeRef NodeRef::next() const{
		if(value->next == nullptr){
			throw NullPtrException();
		}
		return NodeRef(value->next);
	}
	NodeRef NodeRef::prev() const{
		if(value->prev == nullptr){
			throw NullPtrException();
		}
		return NodeRef(value->prev);
	}
	NodeRef NodeRef::parent() const{
		if(value->parent == nullptr){
			throw NullPtrException();
		}
		return NodeRef(value->parent);
	}
	NodeRef NodeRef::child() const{
		if(value->children == nullptr){
			throw NullPtrException();
		}
		return Node(value->children);
	}

	//提取出文本
	std::string NodeRef::text() const{
		xmlChar *mem = XML_GET_CONTENT(value);
		if(mem == nullptr){
			//得到内容失败
			throw NullPtrException();
		}
		return std::string(c_cast<char*>(mem));
	}
	//提取出名字
	std::string NodeRef::name() const{
		if(value->name == nullptr){
			throw NullPtrException();
		}
		return std::string(c_cast<char*>(value->name));
	}
	//到字符串
	std::string NodeRef::to_string() const{
		xmlBufferPtr buf = xmlBufferCreate();
		//创建缓冲区
		xmlNodeDump(buf,value->doc,value,1,1);
		std::string str(c_cast<char*>(buf->content));
		xmlBufferFree(buf);
		return str;
	}
	//提取出属性
	std::string NodeRef::operator[](std::string_view name) const{
		xmlAttrPtr attr = xmlHasProp(value,BAD_CAST name.data());
		if(attr == nullptr){
			throw KeyError(name);
		}
		return c_cast<char*>(XML_GET_CONTENT(attr->children));
	}
	//设置文本
	void NodeRef::set_text(std::string_view text){
		xmlNodeSetContentLen(value,BAD_CAST(text.data()),text.length());
	}
	//设置名字
	void NodeRef::set_name(std::string_view name){
		xmlNodeSetName(value,BAD_CAST(name.data()));
	}
	//添加孩子
	void NodeRef::add_child(Node &&node){
		if(*node.refcount == 1){
			//只有一个引用
			//直接加入
			xmlAddChild(value,node.value);
			delete node.refcount;
			node.refcount --;
			return;
		}
		else{
			//拷贝一下
			xmlAddChild(value,xmlCopyNode(node.value,1));
		}
	}
	void NodeRef::add_child(NodeRef ref){
		xmlAddChild(value,xmlCopyNode(ref.value,1));
	}
	//添加attr
	void NodeRef::add_attr(std::string_view attr,std::string_view value){
		xmlNewProp(this->value,BAD_CAST(attr.data()),BAD_CAST(value.data()));
	}
	//是否有属性
	bool NodeRef::has_attr(std::string_view attr) const{
		return xmlHasProp(value,BAD_CAST(attr.data())) != nullptr;
	}
	XPath::Object NodeRef::xpath(std::string_view exp) const{
		Xml xml = Xml::ParseString(to_string());
		//转换为xml
		return xml.xpath(exp.data());
	}
	//输出到ostream
	std::ostream &operator<<(std::ostream &ostr,NodeRef ref){
		xmlBufferPtr buf = xmlBufferCreate();
		//创建缓冲区
		xmlNodeDump(buf,ref.value->doc,ref.value,1,1);
		ostr.write((c_cast<char*>(buf->content)),buf->use);
		xmlBufferFree(buf);
		return ostr;
	}
	//拷贝
	Node::Node(NodeRef ref):
		Node(xmlCopyNode(ref.value,1)){

	}
	Node::Node(xmlNodePtr ptr):NodeRef(ptr){
		refcount = new int(1);
	}
	Node::Node(const Node &node):NodeRef(node.value){
		//浅拷贝
		refcount = node.refcount;
		if(refcount != nullptr){
			(*refcount) ++;
		}
		else{
			//没有数值 是空的
			value = nullptr;
		}
	}
	Node::Node(Node &&node){
		//移动
		value = node.value;
		refcount = node.refcount;
		node.refcount = nullptr;
	}
	Node::~Node(){
		//销毁数据
		if(refcount != nullptr){
			(*refcount) --;
			if(refcount == 0){
				delete refcount;
				xmlFreeNode(value);
			}
		}
	}
	//复制
	Node Node::clone() const{
		return Node(xmlCopyNode(value,1));		
	}
	
};
};
//Html
namespace Box{
namespace LXml{
	//Html
	Html Html::ParseString(std::string_view str){
		
		htmlDocPtr html = htmlReadMemory(str.data(),str.length(),nullptr,DefaultEncoding,
			XML_PARSE_NOERROR | XML_PARSE_NONET | XML_PARSE_RECOVER | XML_PARSE_NOWARNING);
		//解析字符串
		if(html == nullptr){
			throwError();
		}
		return Html(html);
	}
	std::string Html::to_string() const{
		//到字符串
		xmlChar *mem;
		int size;
		htmlDocDumpMemoryFormat(value,&mem,&size,1);
		std::string str(c_cast<char*>(mem),size);
		xmlFree(mem);
		return str;
	}
	//保存
	void Html::savefile(const char *filename,int fmt,const char *enc){
		if(htmlSaveFileFormat(filename,value,enc,fmt) != 0){
			//失败
			throwError();
		}
	}
	void Html::dump(FILE  *fstream){
		if(htmlDocDump(fstream,value) != 0){
			throwError();
		}
	}
};
};
