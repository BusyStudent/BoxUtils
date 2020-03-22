#include <libxml/xmlstring.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <cstring>
#include "xml.hpp"
#include "xpath.hpp"
#include "exception.hpp"
using namespace Box::XML;
using namespace Box;
const char *Doc::default_version = "1.0";
const char *Doc::default_encoding = "UTF-8";
Doc::Doc(void *ptr){
	//初始化文档
	docptr = ptr;//给个值
}
Doc::Doc(const Doc &doc){
	//拷贝一下
	docptr = xmlCopyDoc((xmlDocPtr)doc.docptr,1);
}
Doc::~Doc(){
	//销毁文档
	xmlFreeDoc((xmlDocPtr)docptr);//释放
}
Doc Doc::Create(const char *version){
	//创建一个文档
	return Doc(xmlNewDoc(BAD_CAST version));
}
Doc Doc::LoadFile(const char *filename){
	//载入文件
	auto ptr = xmlParseFile(filename);
	//记得加个错误检查
	
	return Doc(ptr);
}
Doc Doc::ParseString(const char *str){
	auto ptr = xmlParseMemory(str,strlen(str));
	
	return Doc(ptr);
}
//一些功能
//得到内容
Node Doc::get_root(){
	auto nodeptr = xmlDocGetRootElement((xmlDocPtr)docptr);
	//得到根节点
	if(nodeptr == nullptr){
		//如果为空指针
		throw NullPtrException();
	}
	return Node(nodeptr,false);//不是独立的
}
const char *Doc::get_version(){
	//得到版本
	return (const char *)(((xmlDocPtr)docptr)->version);
}
const char *Doc::get_encoding(){
	//得到编码方式
	return (const char *)(((xmlDocPtr)docptr)->encoding);
}
void *Doc::get_docptr(){
	return docptr;
}

void Doc::set_root(Node &root){
	//设置根节点
	auto nptr = (xmlNodePtr)_Utils::CopyNodePtr(root);
	//自动处理root
	auto ret = xmlDocSetRootElement((xmlDocPtr)docptr,nptr);
	//释放老旧的Root
	if(ret != nullptr){
		xmlFreeNode(ret);
	}
}
void Doc::save_file(const char *filename,int format){
	//保存文件
	xmlSaveFormatFile(filename,(xmlDocPtr)docptr,format);
}
Doc *Doc::clone(){
	return new Doc(*this);
}
Nodes Doc::xpath(const char *exp,bool need_clone){
	return XPath::XPath(*this,exp,need_clone);
}
std::string Doc::to_string(int format){
	//到字符串
	int size;
	xmlChar *mem;
	xmlDocDumpFormatMemory((xmlDocPtr)docptr,&mem,&size,format);
	std::string str((const char*)mem,size);
	xmlFree(mem);
	return str;
}
//
Node::Node(void *nodeptr,bool independence){
	//复制一下指针
	this->nodeptr = nodeptr;
	this->independence = independence;
}
Node::~Node(){
	if(independence == true){
		//只有独立的才会被释放
		xmlFreeNode((xmlNodePtr)nodeptr);
	}
}
void Node::add_text_child(const char *name,const char *contents){
	//添加文本
	xmlNewTextChild((xmlNodePtr)nodeptr,nullptr,BAD_CAST name,BAD_CAST contents);
}
void Node::add_child(Node &child){
	//需要检查child是否独立的
	xmlAddChild((xmlNodePtr)nodeptr,
		(xmlNodePtr)_Utils::CopyNodePtr(child));
}
//内容获取
const char *Node::get_content(){
	//得到内容
	auto p = (xmlNodePtr) nodeptr;
	//直接访问content 不用xmlNodeGetContent
	//应为会malloc一个副本
	return ((const char*)p->content);
}
const char *Node::get_name(){
	//得到名字
	return (const char*)(((xmlNodePtr)nodeptr)->name);
}
unsigned int Node::get_line(){
	return ((xmlNodePtr)nodeptr)->line;
}
void Node::add_attr(const char *name,const char *content){
	//加入特征
	xmlNewProp((xmlNodePtr)nodeptr,BAD_CAST name,BAD_CAST content);
}
bool Node::has_attr(const char *name){
	//是否有属性
	auto ret = xmlHasProp((xmlNodePtr)nodeptr,BAD_CAST name);
	if(ret == nullptr){
		return false;
	}
	else{
		return true;
	}
}
std::string Node::get_attr_content(const char *name){
	//得到特征的名字
	auto s = xmlGetProp((xmlNodePtr)nodeptr,BAD_CAST name);
	if(s == nullptr){
		//没找到
		throw KeyError(name);
	}
	std::string str((const char*)s);
	xmlFree(s);
	return str;
}
std::string Node::operator [](const char *name){
	return Node::get_attr_content(name);
}
std::string Node::text(){
	auto ptr = (xmlNodePtr)nodeptr;
	auto child = ptr->children;
	//text在孩子里面
	if(child == nullptr){
		error:;
		throw Box::NullPtrException();
	}
	auto text = XML_GET_CONTENT(child);
	if(text == nullptr){
		goto error;
	}
	return std::string((const char*)text);
}
void Node::for_attr(std::function <bool(Attr&)> fn){
	//遍历特征
	Attr attr(((xmlNodePtr)nodeptr)->properties,false);
	//遍历链表
	bool b;
	while(attr.attrptr != nullptr){
		//去下一个
		b = fn(attr);
		if(b == false){
			//退出迭代
			return;
		}
		attr.attrptr = ((xmlAttrPtr)attr.attrptr)->next;
	}
}
//设置信息
void Node::set_content(const char *content){
	xmlNodeSetContent((xmlNodePtr)nodeptr,BAD_CAST content);
}
//遍历
void Node::for_each(std::function <bool(Node&)> fn){
	//得到孩子
	bool b;
	Node n(((xmlNodePtr)nodeptr)->children,false);
	while(n.nodeptr != nullptr){
		//迭代下去
		b = fn(n);
		if(b == false){
			//终止迭代
			return;
		}
		n.nodeptr = (((xmlNodePtr)n.nodeptr)->next);
	}
}
//得到位置信息
Node Node::get_children(){
	//得到孩子
	auto ptr = (xmlNodePtr)nodeptr;
	auto children = ptr->children;
	if(children == nullptr){
		throw Box::NullPtrException();
	}
	else{
		return Node(children,false);
	}
}
Node Node::get_prev(){
	//得到前面那个
	auto ptr = (xmlNodePtr)nodeptr;
	auto prev = ptr->prev;
	if(prev == nullptr){
		throw Box::NullPtrException();
	}
	else{
		return Node(prev,false);
	}
}
Node Node::get_next(){
	//得到下一个
	auto ptr = (xmlNodePtr)nodeptr;
	auto next = ptr->next;
	if(next == nullptr){
		throw Box::NullPtrException();
	}
	else{
		return Node(next,false);
	}
}
Node Node::get_parent(){
	//得到上一级
	auto ptr = (xmlNodePtr)nodeptr;
	auto parent = ptr->next;
	if(parent == nullptr){
		throw Box::NullPtrException();
	}
	else{
		return Node(parent,false);
	}
}
//一些静态函数
Node Node::Create(const char *name){
	//创建节点
	return Node(xmlNewNode(nullptr,BAD_CAST name));
}
Node Node::CreateText(const char *content){
	//创建文本
	return Node(xmlNewText(BAD_CAST content));
}
//工具函数
void XML::InitParser(){
	xmlInitParser();
}
void XML::CleanupParser(){
	xmlCleanupParser();
}
void XML::MemoryDump(){
	xmlMemoryDump();
}
void XML::Cleanup(){
	//回收资源
	CleanupParser();
	MemoryDump();
}
void *_Utils::CopyNodePtr(Node &node){
	xmlNodePtr ptr;
	if(node.independence == true){
		//是独立的
		node.independence = false;//改为不独立的
		ptr = (xmlNodePtr)node.nodeptr;
	}
	else{
		ptr = xmlCopyNode((xmlNodePtr)node.nodeptr,1);
	}
	return ptr;
}
//Attr类的实现
Attr::Attr(void *ptr,bool inde){
	//复制一下
	attrptr = ptr;
	independence = inde;
}
Attr::Attr(const Attr &a){
	//不独立
	attrptr = a.attrptr;
	independence = false;
}
Attr::~Attr(){
	//销毁
	if(independence == true){
		//释放
		xmlFreeProp((xmlAttrPtr)attrptr);
	}
}
//得到信息
const char *Attr::get_name(){
	//得到名字
	return (const char *)(((xmlAttrPtr)attrptr)->name);
}
const char *Attr::get_content(){
	auto node = ((xmlAttrPtr)attrptr)->children;
	return (const char*)(node->content);
}
//HTML
HTML HTML::ParseString(const char *str,const char *url,const char *encoding){
	// HTML_PARSE_RECOVER 宽松检查
	auto ret = htmlReadMemory(str,strlen(str),url,encoding,
		HTML_PARSE_RECOVER | HTML_PARSE_COMPACT |
		HTML_PARSE_NONET | HTML_PARSE_NOERROR
		 );
	if(ret == nullptr){
		//解析失败
		throw NullPtrException();
	}	
	return HTML(ret);
}
HTML HTML::LoadFile(const char *filename,const char *encoding){
	auto doc = htmlParseFile(filename,encoding);
	//解析文件
	if(doc == nullptr){
		throw NullPtrException();
	}
	return HTML(doc);
}
HTML::HTML(void *ptr):Doc(ptr){
	//委托给doc的Ptr
}
HTML::HTML(const HTML &h):Doc(h){
	//委托哦给上面
}
HTML HTML::Create(const char *url,const char *id){
	return HTML(htmlNewDoc(BAD_CAST url,BAD_CAST id));
}
void HTML::save_file(const char *filename,int format){
	htmlSaveFileFormat(filename,(htmlDocPtr)docptr,default_encoding,format);
}
std::string HTML::to_string(int format){
	//到字符串
	xmlChar *mem;
	int size;
	htmlDocDumpMemoryFormat((xmlDocPtr)docptr,&mem,&size,format);
	std::string str((const char*)mem,size);
	xmlFree(mem);
	return str;
}
