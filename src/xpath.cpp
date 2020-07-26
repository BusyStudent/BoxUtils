#define _BOX_SOURCE
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <utility>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include "refptr.hpp"
#include "exception.hpp"
#include "xpath.hpp"
#include "xml.hpp"
namespace Box{
namespace LXml{
namespace XPath{
	//释放XPath Object 考虑到有个XMLCALL
	static void free_xpath(xmlXPathObjectPtr objptr){
		if(objptr != nullptr){
			xmlXPathFreeObject(objptr);
		}
	}
	Context::Context(Xml &xml)
		:holder(xml.holder){
		ctxt = xmlXPathNewContext(xml.holder->xml);
		if(ctxt == nullptr){
			//失败
			throw Error(xmlGetLastError());
		}
	}
	Context::~Context(){
		xmlXPathFreeContext(ctxt);
	}
	//移动一下
	Context::Context(Context &&ctxt)
		:holder(ctxt.holder){
		this->ctxt = ctxt.ctxt;
		ctxt.ctxt = nullptr;
	}
	//解析
	Object Context::eval(const char *exp){
		xmlXPathObjectPtr obj = xmlXPathEvalExpression(BAD_CAST(exp),ctxt);
		if(obj == nullptr){
			//解析失败
			throw Error(xmlGetLastError());
		}
		return Object(obj,holder);
	}
	Object Context::eval(const std::string &exp){
		return eval(exp.c_str());
	}
	Object Context::eval(const Expression &exp){
		//解析编译过的表达式
		xmlXPathObjectPtr obj = xmlXPathCompiledEval(exp.expr,ctxt);
		if(obj == nullptr){
			throw Error(xmlGetLastError());
		}
		return Object(obj,holder);
	}
	//编译
	Expression Context::complie(const char *exp){
		xmlXPathCompExprPtr expr =  xmlXPathCtxtCompile(ctxt,BAD_CAST(exp));
		if(expr == nullptr){
			//失败
			throw Error(xmlGetLastError());
		}
		return Expression(expr);
	}
	Expression Context::complie(const std::string &exp){
		return complie(exp.c_str());
	}
	//XPath对象
	Object::Object(xmlXPathObjectPtr objptr,RefPtr <LXml::XmlHolder> &_holder)
		:obj(objptr,free_xpath),holder(_holder){
	}
	//复制
	Object::Object(const Object &o)
		:obj(o.obj),holder(o.holder){

	}
	//移动
	Object::Object(Object &&o)
		:obj(o.obj),holder(o.holder){

	}
	Object::~Object(){

	}
	ObjectIter Object::begin() const{
		//得到开始迭代器
		xmlNodePtr nodeptr = nullptr;
		int where = -1;
		if(not empty()){
			//如果不是空的 得到第一个
			where = 0;
			nodeptr = obj.get()->nodesetval->nodeTab[0];
		}
		return ObjectIter(obj,nodeptr,where);
	}
	ObjectIter Object::end() const{
		//得到末尾
		return ObjectIter(obj,nullptr,-1);
	}
	//得到一些信息
	bool Object::empty() const{
		//是否是空的
		return xmlXPathNodeSetIsEmpty(obj.get()->nodesetval);
	}
	int Object::size() const{
		return obj.get()->nodesetval->nodeNr;	
	}
	template<>
	double Object::get<double>() const{
		return xmlXPathCastToNumber(obj.get());
	}
	template<>
	bool   Object::get<bool>() const{
		return xmlXPathCastToBoolean(obj.get());
	}
	template <>
	std::string Object::get<std::string>() const{
		//得到字符串
		xmlChar *mem = xmlXPathCastToString(obj.get());
		if(mem == nullptr){
			throw NullPtrException();
		}
		std::string str((const char *)mem);
		xmlFree(mem);
		return str;
	}
	//迭代器操作
	ObjectIter::ObjectIter(const RefData <_xmlXPathObject*> &_obj,_xmlNode *nodeptr,int where)
		:obj(_obj),node(nodeptr,true){
		//初始化
		this->where = where;
	}
	ObjectIter::~ObjectIter(){
		//销毁
	}
	//操作符号
	bool ObjectIter::operator ==(const ObjectIter &iter) const{
		//判断指针是否相等
		return node.holder->node == iter.node.holder->node;
	}
	bool ObjectIter::operator !=(const ObjectIter &iter) const{
		//判断指针是否不相等
		return node.holder->node != iter.node.holder->node;
	}
	void ObjectIter::operator++(){
		//递增
		int max = obj.get()->nodesetval->nodeNr;//得到最大的
		if(where + 1 >= max){
			//移动到最后一个
			where = -1;
			node.holder->node = nullptr;
			return;
		}
		else{
			where ++;
			node.holder->node = obj.get()->nodesetval->nodeTab[where];
		}
	}
	void ObjectIter::operator--(){
		int max = obj.get()->nodesetval->nodeNr;//得到最大的
		if(where == -1){
			//如果是最后一个
			where = max - 1;
		}
		else if(where -1 >= 0){
			where --;
		}
		node.holder->node = obj.get()->nodesetval->nodeTab[where];
	}
	//表达式
	Expression::Expression(const char *exp){
		//编译表达式
		expr = xmlXPathCompile(BAD_CAST(exp));
		if(expr == nullptr){
			//失败
			throw Error(xmlGetLastError());
		}
	}
	Expression::Expression(const std::string &exp){
		expr = xmlXPathCompile(BAD_CAST(exp.c_str()));
		if(expr == nullptr){
			//失败
			throw Error(xmlGetLastError());
		}
	}
	Expression::Expression(_xmlXPathCompExpr *ex)
		:expr(ex){

	}
	//转移
	Expression::Expression(Expression &&expr){
		this->expr = expr.expr;
		expr.expr = nullptr;
	}
	Expression::~Expression(){
		if(expr != nullptr){
			xmlXPathFreeCompExpr(expr);
		}
	}
} // namespace LXml::XPath
};
};
