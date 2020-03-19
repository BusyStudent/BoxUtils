#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "exception.hpp"
#include "xpath.hpp"
using namespace Box;
static auto get_xpath_object = [](void *docptr,const char *exp) -> xmlXPathObjectPtr{
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
XPath::Nodes XPath::XPath(XML::Doc &doc,const char *exp){
	//解析xpath解析
	auto cloned_doc = doc.clone();
	auto obj = get_xpath_object(cloned_doc->get_docptr(),exp);
	XPath::NodeVec vec;
	auto nset = obj->nodesetval;
	//得到集合
	if(!xmlXPathNodeSetIsEmpty(nset)){
		//如果集合不为空
		for(int i = 0;i < nset->nodeNr;i++){
			XML::Node node(nset->nodeTab[i],false);
			vec.push_back(node);
		}
		//压入结果
	}
	//删除xpath对象
	xmlXPathFreeObject(obj);
	return {
		//返回内容
		.cloned_doc = cloned_doc,
		.vec = vec,
	};
}
//Nodes
XPath::Nodes::~Nodes(){
	//销毁克隆的doc
	delete ((XML::Doc*)cloned_doc);
}
//语法糖
XPath::NodeVec &XPath::Nodes::operator *(){
	return vec;
}
XPath::NodeVec *XPath::Nodes::operator ->(){
	return &vec;
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
