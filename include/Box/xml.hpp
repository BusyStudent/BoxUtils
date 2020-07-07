#ifndef _BOX_XML_HPP_
#define _BOX_XML_HPP_
#include <cstdio>
#include <string>
#include <iosfwd>
#include <exception>
#include "refptr.hpp"
struct _xmlDoc;
struct _xmlNode;
struct _xmlXPathObject;
struct _xmlError;
namespace Box{
	namespace LXml{
		//Lib XML的命名空间
		namespace XPath{
			class Context;
			class Object;
			class ObjectIter;
		};
		extern const char *DefaultVersion;//默认的版本
		extern const char *DefaultEncoding;//默认编码方式
		class Error:public std::exception{
			//错误
			public:
				Error(_xmlError *errptr);
				~Error();
				inline _xmlError* get() const {
					//得到错误指针
					return error;
				};
				const char *what() const throw();
			private:
				_xmlError *error;//错误的指针
		};
		struct XmlHolder{
			//XML包裹器
			~XmlHolder();
			_xmlDoc *xml;
			bool ref;
		};
		struct NodeHolder{
			//Node的包裹器
			~NodeHolder();
			inline operator _xmlNode*();
			_xmlNode *node;
			bool ref;
		};
		class Node{
			//节点
			public:
				Node(_xmlNode *node,bool ref = false);
				Node(const Node &);
				Node(Node &&);
				~Node();
				Node copy()   const;//复制一份一摸一样的
				//得到其他节点引用或者其他东西
				Node prev()    const;
				Node next()    const;
				Node child()   const;
				Node parent()  const;
				bool is_ref()  const;//是否是引用
				bool is_text() const;//是否是文字
				std::string name() const;//得到名字
				std::string text() const;//得到文字
				std::string to_string() const;//到字符串
				std::string operator [](const char *attr) const;//提取属性
				std::string operator [](const std::string &attr) const;//提取属性

				void set_text(const std::string &text);//设置文本
				void set_text(const char *text);//设置文本

				void set_name(const std::string &name);//设置名字
				void set_name(const char *name);
				//添加孩子 所有权会被转移
				void add_child(Node &&node);
				void add_child(Node &node);
				//添加属性
				void add_attr(const char *attr,const char *value);
				//是否有属性
				bool has_attr(const char *attr  ) const;
				bool has_attr(const std::string&) const;
				//XPath
				XPath::Object xpath(const char *exp);
				XPath::Object xpath(const std::string &exp);
				//输出ostream
				friend std::ostream &operator <<(std::ostream&,const Node&);
			private:
				RefPtr<NodeHolder> holder;
			friend class Xml;
			friend class Html;
			friend class XPath::ObjectIter;
		};
		class Html;
		class Xml{
			public:
				typedef LXml::Node Node;
				Xml(const char *version = DefaultVersion);
				//创建一个Xml
				Xml(_xmlDoc *xml,bool ref = false);
				//从xml指针构建 默认不是引用
				Xml(const Xml &);
				Xml(Xml &&);
				~Xml();
				bool is_ref() const;//是否是引用
				std::string version()  const;//得到版本
				std::string encoding() const;//得到编码方式
				std::string to_string()const;//转换为字符串
				Node root() const;//得到根节点
				void set_root(Node &&node);//设置根节点 所有权会被转移
				void set_root(Node &node);//和上面一样

				void savefile(const char *file,int fmt,const char *enc = DefaultEncoding);//保存一下
				void dump(FILE *fstream);//保存到文件流
				//XPath
				XPath::Object xpath(const char *exp);
				XPath::Object xpath(const std::string &exp);
				//输出ostream
				friend std::ostream & operator<<(std::ostream&,const Xml&);
				friend std::ostream & operator<<(std::ostream&,const Html&);
				static Xml ParseString(const char *str);//解析字符串
				static Xml ParseString(const std::string &str);
				static Xml LoadFile(const char *filename);//加载文件
			private:
				RefPtr<XmlHolder>  holder;
			friend class Html;
			friend class XPath::Context;
			//friend class XPath::ObjectIter;
		};
		class Html:public Xml{
			public:
				typedef LXml::Node Node;
				using Xml::Xml;

				Html(const char *url = nullptr,const char *enc = DefaultEncoding);
				//生成一个Html
				std::string to_string() const;//得到字符串
				//保存
				void savefile(const char *file,int fmt,const char *enc = DefaultEncoding);
				void dump(FILE *fstream);
				//解析字符串
				
				static Html ParseString(const char *str);
				static Html ParseString(const std::string &);
		};
		void Init();//初始化
		void Quit();//退出
		void *Malloc(size_t byte);
		void  Free(void *mem);
	};
	typedef LXml::Xml Xml;
	typedef LXml::Html Html;
};
#endif
