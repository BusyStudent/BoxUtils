#include <libxml/xmlstring.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <utility>
#include <cstring>
#include <cassert>
#include "xml.hpp"
#include "xpath.hpp"
#include "exception.hpp"
using namespace Box::XML;
using namespace Box;
//Doc
const char *Doc::default_version = "1.0";//默认版本
const char *Doc::default_encoding = "UTF-8";//默认编码
void XML::ThrowLastError(){
	auto err = xmlGetLastError();
	if(err == nullptr){
		//不知道错误
		throw std::runtime_error("ParseError");
	}
	throw std::runtime_error(err->message);
}
Doc::Doc(_xmlDoc *docptr){
	//传个指针创建
	this->docptr = docptr;
}
Doc::Doc(const Doc &doc){
	//复制文档
	docptr = xmlCopyDoc(doc.docptr,1);
}
Doc::Doc(Doc &&doc){
	//转移
	docptr = doc.docptr;
	doc.docptr = nullptr;
}
Doc::Doc(const char *version){
	//自己创建一个
	docptr = xmlNewDoc(BAD_CAST version);
}
Doc::~Doc(){
	xmlFreeDoc(docptr);
}
//转换字符串
std::string Doc::to_string(int fmt){
	xmlChar *mem;//内存
	int size;//大小
	xmlDocDumpFormatMemoryEnc(docptr,&mem,&size,default_encoding,fmt);
	std::string str((const char*)mem,size);
	xmlFree(mem);
	return str;
}
//得到根节点
Node Doc::root(){
	xmlNodePtr root = xmlDocGetRootElement(docptr);//得到根节点
	if(root == nullptr){
		throw NullPtrException();
	}
	return Node(root,false);
}
bool Doc::savefile(const char *filename,int fmt){
	//保存文件
	return xmlSaveFormatFileEnc(filename,docptr,default_encoding,fmt) == 0;
}
bool Doc::dump(FILE *fptr){
	//存到文件里面
	return xmlDocDump(fptr,docptr) == 0;
}
void Doc::set_root(Node &&root){
	//设置根节点
	xmlNodePtr oldnode;
	if(root.independence){
		//是独立节点
		root.independence = false;//不是独立的
		oldnode = xmlDocSetRootElement(docptr,root.nodeptr);
	}
	else{
		oldnode = xmlDocSetRootElement(docptr,xmlCopyNode(root.nodeptr,1));
	}
	xmlFreeNode(oldnode);
	//释放老式节点
}
void Doc::set_root(Node &root){
	//设置根节点
	set_root(std::move(root));
}
//解析字符串
Doc Doc::ParseString(const char *text,int len){
	if(len == 0){
		//如果长度为0
		len = strlen(text);
	}
	xmlDocPtr docptr = xmlParseMemory(text,len);
	if(docptr == nullptr){
		//失败
		ThrowLastError();
	}
	return Doc(docptr);
}
Doc Doc::LoadFile(const char *filename){
	xmlDocPtr docptr = xmlParseFile(filename);
	if(docptr == nullptr){
		ThrowLastError();
	}
	return Doc(docptr);
}
//Node
Node::Node(xmlNodePtr nodeptr,bool independence){
	this->nodeptr = nodeptr;
	this->independence = independence;
}
Node::Node(const char *name){
	nodeptr = xmlNewNode(nullptr,BAD_CAST name);
	independence = true;//是独立的
}
//文本节点
Node::Node(const char *name,const char *text){
	nodeptr = xmlNewNode(nullptr,BAD_CAST name);
	xmlNodeSetContent(nodeptr,BAD_CAST text);
	independence = true;
}
Node::Node(const Node &node){
	//浅拷贝
	nodeptr = node.nodeptr;
	independence = false;
}
Node::Node(Node &&node){
	if(node.independence){
		//是独立节点
		node.independence = false;
		nodeptr = node.nodeptr;
	}
	else{
		//不是
		independence = false;
		nodeptr = node.nodeptr;
	}
}
Node::~Node(){
	if(independence){
		//如果是独立的
		xmlFreeNode(nodeptr);
	}
}

//查找属性
//名字
std::string Node::name(){
	return std::string((const char*)nodeptr->name);
}
std::string Node::content(){
	//这个节点的内容
	xmlChar *text = xmlNodeGetContent(nodeptr);
	if(text == nullptr){
		throw NullPtrException();
	}
	std::string str((const char*)text);
	xmlFree(text);
	return str;
}
std::string Node::text(){
	//得到child->content
	if(nodeptr->children == nullptr){
		//没有下一个节点
		throw NullPtrException();
	}
	return std::string((const char*)nodeptr->children->content);
}
std::string Node::path(){
	//得到节点路径
	xmlChar *str = xmlGetNodePath(nodeptr);
	if(str == nullptr){
		throw NullPtrException();
	}
	std::string s((const char *)str);
	xmlFree(str);
	return s;
}
std::string Node::operator [](const char *name){
	xmlAttrPtr attr = xmlHasProp(nodeptr,BAD_CAST name);
	if(attr == nullptr){
		throw KeyError(name);
	}
	return std::string((const char*)attr->children->content);//找到属性值
}
//转化到字符串
std::string Node::to_string(int fmt){
	//创建缓冲区域
	xmlBufferPtr buffer = xmlBufferCreate();
	int size = xmlNodeDump(buffer,nodeptr->doc,nodeptr,0,fmt);
	std::string str((const char*)buffer->content,size);
	xmlBufferFree(buffer);//关闭
	return str;
}
//得到行号
long Node::line(){
	return xmlGetLineNo(nodeptr);
}
//是不是空白节点
bool Node::is_blank(){
	return xmlIsBlankNode(nodeptr);
}
//是不是文本节点
bool Node::is_text(){
	return xmlNodeIsText(nodeptr);
}
//取消与其他节点的连接
void Node::unlink(){
	xmlUnlinkNode(nodeptr);
	independence = true;//独立了
}
//是否有这个特征
bool Node::has_attr(const char *name){
	return xmlHasProp(nodeptr,BAD_CAST name) != nullptr;
}
//添加特征
void Node::add_attr(const char *name,const char *value){
	xmlSetProp(nodeptr,BAD_CAST name,BAD_CAST value);
}
//移除特征
bool Node::remove_attr(const char *name){
	return xmlUnsetProp(nodeptr,BAD_CAST name) == 0;
} 
//设置属性
void Node::set_name(const char *name){
	//名字
	xmlNodeSetName(nodeptr,BAD_CAST name);
}
//设置内容
void Node::set_content(const char *content){
	xmlNodeSetContent(nodeptr,BAD_CAST content);
}
//添加文本
void Node::add_content(const char *content){
	xmlNodeAddContent(nodeptr,BAD_CAST content);
}
void Node::add_content(const char *content,int len){
	xmlNodeAddContentLen(nodeptr,BAD_CAST content,len);
}
//节点之间的关系
Node Node::prev() const{
	//前一个节点
	xmlNode *prev = nodeptr->prev;
	if(prev == nullptr){
		throw NullPtrException();
	}
	return Node(prev,false);
}
Node Node::next() const{
	//得到下一个
	if(nodeptr->next == nullptr){
		throw NullPtrException();
	}
	return Node(nodeptr->next,false);
}
Node Node::parent() const{
	//得到家长
	if(nodeptr->parent == nullptr){
		throw NullPtrException();
	}
	return Node(nodeptr->parent,false);
}
Node Node::child() const{
	//得到孩子
	if(nodeptr->children == nullptr){
		throw NullPtrException();
	}
	return Node(nodeptr->children,false);
}
//添加子节点
void Node::add_child(Node &&node){
	xmlNodePtr ret;//返回值
	if(node.independence){
		//独立节点
		ret = xmlAddChild(nodeptr,node.nodeptr);
		node.independence = false;//不是独立的
	}
	else{
		//拷贝添加进去
		ret = xmlAddChild(nodeptr,xmlCopyNode(node.nodeptr,1));
	}
	assert(ret != nullptr);
}
void Node::add_child(Node &node){
	add_child(std::move(node));
}
//添加节点后面
void Node::add_next(Node &&node){
	xmlNodePtr ret;//返回值
	if(node.independence){
		//独立节点
		ret = xmlAddNextSibling(nodeptr,node.nodeptr);
		node.independence = false;//不是独立的
	}
	else{
		//拷贝添加进去
		ret = xmlAddNextSibling(nodeptr,xmlCopyNode(node.nodeptr,1));
	}
	assert(ret != nullptr);
}
void Node::add_next(Node &node){
	add_next(std::move(node));
}
//后置前面
void Node::add_prev(Node &&node){
	xmlNodePtr ret;//返回值
	if(node.independence){
		//独立节点
		ret = xmlAddPrevSibling(nodeptr,node.nodeptr);
		node.independence = false;//不是独立的
	}
	else{
		//拷贝添加进去
		ret = xmlAddPrevSibling(nodeptr,xmlCopyNode(node.nodeptr,1));
	}
	assert(ret != nullptr);
}
void Node::add_prev(Node &node){
	add_prev(std::move(node));
}
//节点复制
Node Node::copy(){
	return Node(xmlCopyNode(nodeptr,1),true);
}
Node *Node::clone(){
	return new Node(xmlCopyNode(nodeptr,1),true);
}
//得到节点指针
xmlNodePtr Node::get_node() const{
	return nodeptr;
}
//统计孩子数量
unsigned int Node::count_child() const{
	unsigned int val = 0;
	if(nodeptr->children == nullptr){
		//没有孩子
		return val;
	}
	for(xmlNodePtr next = nodeptr->children;next != nullptr;next = next->next){
		val++;
	}
	return val;
}
//统计特征
unsigned int Node::count_attr() const{
	unsigned int val = 0;
	if(nodeptr->properties == nullptr){
		//没有孩子
		return val;
	}
	for(xmlAttrPtr next = nodeptr->properties;next != nullptr;next = next->next){
		val++;
	}
	return val;
}
//赋值重载符号
Node &Node::operator =(Node &&node){
	//左边是个右值
	if(this != &node){
		//避免自己给自己赋值
		if(independence){
			//如果自己是独立的
			//释放内存
			xmlFreeNode(nodeptr);
		}
		if(node.independence == true){
			//是独立的
			//把节点改为不独立
			node.independence = false;
			nodeptr = node.nodeptr;//复制指针
			independence = true;//自己变独立
		}
		else{
			//不独立
			//直接改指针
			nodeptr = node.nodeptr;
			independence = false;
		}
	}
	return *this;
}
Node &Node::operator =(Node &node){
	//如果左边是个左值
	//把它当右值看待
	return Node::operator =(std::move(node));
}
//初始化
void XML::Init(){
	xmlInitGlobals();
	xmlInitParser();
}
void XML::Quit(){
	xmlCleanupParser();
	xmlCleanupGlobals();
}
//HTML
HTML::HTML(xmlDocPtr ptr):XML::Doc(ptr){
	//构建函数
}
HTML::HTML(const char *url,const char *ext_id):
	XML::Doc(htmlNewDoc(BAD_CAST url,BAD_CAST ext_id)){
	//构建文档
}
HTML HTML::ParseString(const char *str,int len){
	//解析度字符串
	if(len == 0){
		//自己量出长度
		len = strlen(str);
	}
	xmlDocPtr docptr = htmlReadMemory(str,len,nullptr,default_encoding,
		HTML_PARSE_NONET | HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR |
		HTML_PARSE_RECOVER);
	//解析字符串
	if(docptr == nullptr){
		//失败
		XML::ThrowLastError();
	}
	return HTML(docptr);
}
HTML HTML::LoadFile(const char *filename){
	xmlDocPtr ptr = htmlParseFile(filename,default_encoding);//加载文件
	if(ptr == nullptr){
		//失败
		XML::ThrowLastError();
	}
	return HTML(ptr);
}
bool HTML::dump(FILE *fptr){
	return htmlDocDump(fptr,docptr) == 0;
}
bool HTML::savefile(const char *file,int fmt){
	//保存文件
	return htmlSaveFileFormat(file,docptr,default_encoding,fmt) == 0;
}
std::string HTML::to_string(int fmt){
	//到字符串
	xmlChar *mem;
	int size;
	htmlDocDumpMemoryFormat(docptr,&mem,&size,fmt);
	if(mem == nullptr){
		//失败
		XML::ThrowLastError();
	}
	std::string str((const char*)mem,size);
	xmlFree(mem);
	return str;
}
