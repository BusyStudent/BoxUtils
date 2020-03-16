#ifndef _BOX_XPATH_HPP_
#define _BOX_XPATH_HPP_
//XPath表达式
#include <vector>
#include "xml.hpp"
namespace Box{
	namespace XPath{
		typedef std::vector <Box::XML::Node> NodeVec;
		struct Nodes{
			~Nodes();//销毁器
			//一些函数
			//一些语法糖
			NodeVec *operator ->();
			NodeVec &operator * ();
			Box::XML::Node & operator [](unsigned int);
			
			void *objptr;
			NodeVec vec;//向量
		};
		Nodes XPath(Box::XML::Doc &doc,const char *exp);//解析
	};
};
#endif
