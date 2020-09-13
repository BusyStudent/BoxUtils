#ifndef _BOX_XML_HPP_
#define _BOX_XML_HPP_
#include <cstdio>
#include <string>
#include <iosfwd>
#include <exception>
#include <string_view>
#include "libc/attr.h"
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
		class BOXAPI Error:public std::exception{
			//错误
			public:
				Error(_xmlError *errptr);
				Error(Error &&);
				Error(const Error &);
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
		class Xml;
		class Html;
		class Node;
		class NodeRef;
		class XmlRef;
		
		class BOXAPI NodeRef{
			public:
				NodeRef():value(nullptr){};
				NodeRef(const NodeRef &ref) = default;
				NodeRef(_xmlNode *ptr):value(ptr){};
				//得到其他节点引用或者其他东西
				NodeRef prev()    const;
				NodeRef next()    const;
				NodeRef child()   const;
				NodeRef parent()  const;
				bool is_ref()  const;//是否是引用
				bool is_text() const;//是否是文字
				std::string name() const;//得到名字
				std::string text() const;//得到文字
				std::string to_string() const;//到字符串
				std::string operator [](std::string_view attr) const;//提取属性
				//设置
				void set_text(std::string_view text);
				void set_name(std::string_view name);
				//添加孩子
				void add_child(NodeRef ref);//这个会拷贝一份
				void add_child(Node &&);
				void add_text(std::string_view text);//添加文本
				//属性相关
				bool has_attr(std::string_view attr) const;
				void add_attr(std::string_view attr,std::string_view value);
				//XPath
				XPath::Object xpath(std::string_view exp) const;
				friend std::ostream &operator <<(std::ostream&,NodeRef);
			private:
				_xmlNode *value;
			friend class Node;
			friend class XmlRef;
			friend class Xml;
			friend class XPath::ObjectIter;
		};
		class BOXAPI Node:public NodeRef{
			//节点
			public:
				Node(NodeRef);//拷贝
				Node():NodeRef(),refcount(nullptr){};//空节点
				Node(_xmlNode *ptr);//通过节点指针赋值
				Node(std::string_view text);//文本节点
				Node(const Node &);//浅复制
				Node(Node &&);
				~Node();
				//输出ostream
				Node clone() const;//克隆一份
				Node copy()  const{
					//之前的API
					return clone();
				};
			private:
				int *refcount;//引用计数器
			friend class Xml;
			friend class XmlRef;
			friend class Html;
			friend class NodeRef;
		};
		class Html;
		class BOXAPI XmlRef{
			public:
				typedef LXml::NodeRef NodeRef;
				typedef LXml::Node    Node;
				XmlRef():value(nullptr){};
				XmlRef(_xmlDoc *ptr):value(ptr){};

				std::string version()  const;//得到版本
				std::string encoding() const;//得到编码方式
				std::string to_string()const;//转换为字符串
				
				NodeRef root() const;//得到根节点

				void set_root(Node &&node);//设置根节点 所有权会被转移
				void set_root(NodeRef);

				void savefile(const char *file,int fmt,const char *enc = DefaultEncoding);//保存一下
				void dump(FILE *fstream);//保存到文件流
				//XPath
				XPath::Object xpath(std::string_view exp) const;

				//输出ostream
				friend std::ostream & operator<<(std::ostream&,XmlRef);
				//friend std::ostream & operator<<(std::ostream&,const Html&);
			private:
				_xmlDoc *value;
			friend class Xml;
			friend class Html;
			friend class XPath::Context;
			//friend class XPath::ObjectIter;
		};
		class BOXAPI Xml:public XmlRef{
			public:
				//空构造
				struct EmptyConstruct{};
				Xml(EmptyConstruct):XmlRef(),refcount(nullptr){};

				Xml();//构造一个新的
				Xml(std::string_view version);//构造一个新的 记得给版本号
				Xml(const Xml &);
				Xml(Xml &&);
				Xml(_xmlDoc *ptr);
				Xml(XmlRef);//复制一份
				~Xml();
				//复制一份
				Xml clone() const;
				//XPath 这个更安全
				XPath::Object xpath(std::string_view exp) const;
				static Xml ParseString(std::string_view str);//解析字符串
				static Xml LoadFile(std::string_view filename);//加载文件
			private:
				
				//空的构造
				int *refcount;
			friend class XPath::Context;
			friend class XPath::Object;
		};
		class BOXAPI Html:public Xml{
			public:
				typedef LXml::NodeRef NodeRef;
				typedef LXml::Node    Node;
				using Xml::Xml;

				Html(const char *url = nullptr,const char *enc = DefaultEncoding);
				//生成一个Html
				std::string to_string() const;//得到字符串
				//保存
				void savefile(const char *file,int fmt,const char *enc = DefaultEncoding);
				void dump(FILE *fstream);
				//解析字符串
				
				static Html ParseString(std::string_view str);
		};
		extern void Init();//初始化
		extern void Quit();//退出
		extern void *Malloc(size_t byte);
		extern void  Free(void *mem);
		//抛出异常
		BOXAPI [[noreturn]] void throwError();
		BOXAPI [[noreturn]] void throwError(_xmlError *errptr);
	};
	namespace Fmt{
		//两个格式化的东西
		inline std::string ToString(const LXml::Xml &xml){
			return xml.to_string();
		}
		inline std::string ToString(const LXml::Node &node){
			return node.to_string();
		}
	};
	typedef LXml::Xml Xml;
	typedef LXml::Html Html;
};
#endif
