#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "exception.hpp"
#include "xpath.hpp"
using namespace Box;
static xmlXPathObjectPtr get_xpath_object(void *docptr,const char *exp){
	//得到XPath的表达式
	auto context = xmlXPathNewContext((xmlDocPtr)docptr);//兴建一个XPath的内容
	if(context == nullptr){
		//失败
		//
		throw std::invalid_argument("非法表达式");
	}
	auto obj = xmlXPathEvalExpression(BAD_CAST exp,context);
	//释放上面的内容
	xmlXPathFreeContext(context);
	if(obj == nullptr){
		//失败
		throw std::invalid_argument("非法表达式");
	}
	return obj;
};
XPath::Nodes XPath::XPath(XML::Doc &doc,const char *exp,bool need_clone){
	//解析xpath解析
	XML::Doc *docptr;
	xmlXPathObjectPtr obj;
	if(need_clone == true){
		//需要克隆
		docptr = doc.clone();
		obj = get_xpath_object(docptr->get_docptr(),exp);
	}
	else{
		//不用克隆
		//直接给个nullptr
		docptr = nullptr;
		obj = get_xpath_object(doc.get_docptr(),exp);
	}
	
	XPath::NodeVec vec;
	auto nset = obj->nodesetval;
	//得到集合
	if(!xmlXPathNodeSetIsEmpty(nset)){
		//如果集合不为空
		for(int i = 0;i < nset->nodeNr;i++){
			vec.push_back(XML::Node(nset->nodeTab[i],false));
		}
		//压入结果
	}
	return {
		//返回内容
		.objptr = obj,
		.cloned_doc = docptr,
		//应为delete nullptr是安全的那就就没事
		.vec = vec
	};
}
//Nodes
XPath::Nodes::~Nodes(){
	//销毁克隆的doc
	delete ((XML::Doc*)cloned_doc);
	xmlXPathFreeObject((xmlXPathObjectPtr)objptr);//这里释放xpath对象
}
//语法糖
XPath::NodeVec &XPath::Nodes::operator *(){
	return vec;
}
XPath::NodeVec *XPath::Nodes::operator ->(){
	return &vec;
}
XPath::NodesType XPath::Nodes::type(){
	//得到类型
	auto type = ((xmlXPathObjectPtr)objptr)->type;
	switch(type){
		//转换libxml的类型
		case XPATH_UNDEFINED:
			return XPath::NodesType::UNDEFINED;
		case XPATH_NODESET:
			return XPath::NodesType::SET;
		case XPATH_STRING:
			return XPath::NodesType::STRING;
		case XPATH_NUMBER:
			return XPath::NodesType::NUMBER;
		default:
			return XPath::NodesType::UNDEFINED;
	}
}
std::string XPath::Nodes::to_string(){
	//转化字符串
	auto obj = (xmlXPathObjectPtr)objptr;
	xmlChar *str = xmlXPathCastToString(obj);
	//转换一下
	if(str == nullptr){
		throw Box::NullPtrException();
	}
	else{
		std::string s((char*)str);
		xmlFree(str);
		return s;
	}
}
double XPath::Nodes::to_number(){
	auto obj = (xmlXPathObjectPtr)objptr;
	return xmlXPathCastToNumber(obj);
}
bool XPath::Nodes::to_bool(){
	auto obj = (xmlXPathObjectPtr)objptr;
	return xmlXPathCastToBoolean(obj);
}
XML::Node &XPath::Nodes::operator [](unsigned int i){
	//越界检查
	if(i >= vec.size()){
		throw IndexError(i);
	}
	else{
		return vec[i];
	}
}
XPath::Nodes XML::Node::xpath(const char *exp){
	//节点查找xpath
	auto node = (xmlNodePtr)nodeptr;
	auto obj = get_xpath_object(node->doc,exp);
	//得到xpath节点
	XPath::NodeVec vec;
	xmlNodeSetPtr nset = obj->nodesetval;
	if(!xmlXPathNodeSetIsEmpty(nset)){
		//加入数据
		for(int i = 0;i < nset->nodeNr;i++){
			vec.push_back(XML::Node(nset->nodeTab[i],false));
		}
	}
	//返回结果
	return {
		.objptr = obj,
		.cloned_doc = nullptr,
		.vec = vec
	};
}
