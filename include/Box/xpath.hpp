#ifndef _BOX_XPATH_HPP_
#define _BOX_XPATH_HPP_
//XPath表达式
#include <string>
#include "libc/attr.h"
#include "xml.hpp"
#include "dataguard.hpp"
struct _xmlXPathContext;
struct _xmlXPathObject;
struct _xmlXPathCompExpr;
struct _xmlNodeSet;
namespace Box{
	namespace LXml{
		class Xml;
		class Html;
		namespace XPath{
			class Object;
			class Expression;
			class BOXAPI Context{
				//内容
				public:
					Context(Xml &);
					Context(const Context &) = delete;
					Context(Context &&);
					~Context();
					Object eval(const char *str);//解析字符串并查找
					Object eval(const std::string &str);//解析字符串并查找
					Object eval(const Expression &);

					Expression complie(const char *exp);//编译表达式
					Expression complie(const std::string &);//编译表达式
				private:
					_xmlXPathContext *ctxt;//内容
					RefPtr <LXml::XmlHolder> holder;//包裹Xml
					//防止xml超出范围后销毁掉 使结果失效
			};
			class BOXAPI ObjectIter{
				//迭代器
				public:
					ObjectIter(const RefData <_xmlXPathObject*> &,_xmlNode *node,int where);
					~ObjectIter();
					//解和其他操作
					inline Node &operator *(){
						return node;
					};
					inline Node *operator ->(){
						return &node;
					};
					void operator ++();//递增
					void operator --();//递减
					bool operator ==(const ObjectIter &) const;
					bool operator !=(const ObjectIter &) const;
				private:
					RefData <_xmlXPathObject*> obj;
					Node node;
					int where;//所在位置
			};
			class BOXAPI Object{
				//得到的结果
				public:
					typedef ObjectIter iterator;
					typedef ObjectIter Iterator;
					Object(_xmlXPathObject *,RefPtr <LXml::XmlHolder> &);
					Object(const Object &);
					Object(Object &&);
					~Object();
					//迭代器
					ObjectIter begin() const;
					ObjectIter end()   const;
					bool empty() const;//是否是空的
					int  size()  const;//得到大小
					//得到里面数据
					template<typename T>
					void collect_to(T &vec){
						//收集进入
						for(auto &node:*this){
							vec.push_back(node);
						}
					};
					template<typename T>
					T get() const;
					//隐式转换
					template<typename T>
					inline operator T(){
						return get<T>;
					};
				private:
					RefData <_xmlXPathObject*> obj;
					RefPtr <LXml::XmlHolder> holder;
					//防止xml超出范围后销毁掉 使结果失效
			};
			class BOXAPI Expression{
				//表达式
				public:
					Expression(const char *exp);
					Expression(const std::string &exp);
					Expression(const Expression &) = delete;
					Expression(Expression &&);
					Expression(_xmlXPathCompExpr *);
					~Expression();
				private:
					_xmlXPathCompExpr *expr;
				friend class Context;
			};
			template<>
			double Object::get() const;
			template<>
			bool   Object::get() const;
			template<>
			std::string Object::get() const;
		};
	};
	namespace XPath = LXml::XPath;
};
#endif
