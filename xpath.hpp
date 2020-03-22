#ifndef _BOX_XPATH_HPP_
#define _BOX_XPATH_HPP_
//XPath表达式
#include <vector>
#include "xml.hpp"
namespace Box{
	namespace XPath{
		typedef Box::XML::NodeVec NodeVec;
		typedef Box::XML::Nodes Nodes;
		typedef Box::XML::NodesType NodesType;
		//兼容老式的代码
		Nodes XPath(Box::XML::Doc &doc,const char *exp,bool need_clone = true);//解析
		//默认是需要克隆的(安全考虑)
		//当doc的生命周期比返回结果长时候 可以不克隆
	};
};
#endif
