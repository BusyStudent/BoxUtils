#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <utility>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include "exception.hpp"
#include "xpath.hpp"
using namespace Box::XPath;
using namespace Box::XML;
//XPathObject具体实现
NodeSet &XPathObject::operator *(){
	return vec;
}
NodeSet *XPathObject::operator ->(){
	return &vec;
}
//一些构造函数
XPathObject::XPathObject(xmlXPathObjectPtr ptr,xmlDocPtr docptr){
	typedef XPathObjectType Type;
	obj = ptr;
	this->docptr = docptr;
	xmlNodeSetPtr nodeset = ptr->nodesetval;//节点集合
	if(nodeset != nullptr){
		//如果集合数据不为nullptr
		for(int i = 0;i < nodeset->nodeNr;i++){
			//加入节点
			vec.push_back(Node(nodeset->nodeTab[i],false));
		}
	}
	switch(obj->type){
		case XPATH_BOOLEAN:{
			//是Bool
			_type = Type::BOOL;
			break;
		}
		case XPATH_NUMBER:{
			//是数字
			_type = Type::FLOAT;
			break;
		}
		case XPATH_STRING:{
			//字符串
			_type = Type::STRING;
			break;
		}
		case XPATH_NODESET:{
			//集合
			_type = Type::SET;
			break;
		}
		default:{
			//未知
			_type = Type::UNKNWON;
		}
	}
}
XPathObject::XPathObject(XPathObject &&xobj){
	//移动构造函数
	obj = xobj.obj;//转移指针
	xobj.obj = nullptr;
	vec = std::move(xobj.vec);//移动Vector
	_type = xobj._type;//复制类型
	docptr = xobj.docptr;//转移docptr
	xobj.docptr = nullptr;
}
XPathObject::XPathObject(const XPathObject &xobj){
	//复制函数
	obj = xmlXPathObjectCopy(xobj.obj);
	for(auto &node:xobj.vec){
		vec.push_back(node);
	}
	_type = xobj._type;
	if(xobj.docptr != nullptr){
		//里面有一份被复制的doc
		docptr = xmlCopyDoc(xobj.docptr,1);
	}
	else{
		docptr = nullptr;
	}
}
XPathObject::~XPathObject(){
	xmlXPathFreeObject(obj);
	if(docptr != nullptr){
		//里面有一封被克隆的文档
		xmlFreeDoc(docptr);
	}
}
//转换函数
XPathObject::operator std::string(){
	xmlChar *str = xmlXPathCastToString(obj);
	//转换一下
	if(str == nullptr){
		//失败
		throw NullPtrException();
	}
	std::string s((const char*)str);
	xmlFree(str);
	return s;
}
XPathObject::operator bool(){
	return xmlXPathCastToBoolean(obj);
}
XPathObject::operator double(){
	return xmlXPathCastToNumber(obj);
}
//得到里面的vec
NodeSet &XPathObject::get_vec(){
	return vec;
}
//查找节点
Node &XPathObject::operator [](int val){
	auto size = vec.size();//得到大小
	if(abs(val) >= size){
		//超出范围
		if(val > 0){
			throw IndexError(val);
		}
	}
	if(val >= 0){
		//正的index
		return vec[val];
	}
	else{
		//负数
		return vec[size - (-val)];
	}
}
//得到节点数目
unsigned XPathObject::size(){
	return vec.size();
}
std::string XPathObject::to_string(){
	return XPathObject::operator std::string();
}
double XPathObject::to_double(){
	return XPathObject::operator double();
}
bool XPathObject::to_bool(){
	return XPathObject::operator bool();
}
//Doc的XPath实现
XPathObject Doc::xpath(const char *exp,bool clone_doc){
	xmlDocPtr ptr = docptr;
	if(clone_doc){
		//如果要克隆自己
		ptr = xmlCopyDoc(docptr,1);
	}
	auto context = xmlXPathNewContext(ptr);
	//构建上下文
	if(context == nullptr){
		throw NullPtrException();
	}
	auto val = xmlXPathEvalExpression(BAD_CAST exp,context);
	xmlXPathFreeContext(context);
	if(val == nullptr){
		//失败
		if(clone_doc){
			xmlFreeDoc(ptr);
		}
		XML::ThrowLastError();
	}
	if(clone_doc == true){
		//是克隆过的
		return XPathObject(val,ptr);
	}
	return XPathObject(val);
}
//Node XPath实现
XPathObject Node::xpath(const char *exp){
	auto doc = XML::Doc::ParseString(this->to_string().c_str());
	//先把自己转换为doc
	return doc.xpath(exp,true);
}
