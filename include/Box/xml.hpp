#ifndef _BOX_XML_HPP_
#define _BOX_XML_HPP_
#include <cstdio>
#include <string>
#include <vector>
struct _xmlDoc;
struct _xmlNode;
struct _xmlXPathObject;
namespace Box{
	namespace XML{
		//XML
		class Node;
		typedef std::vector <Node> NodeSet;//Node的向量
		enum class XPathObjectType{
			SET,//集合
			STRING,//字符串
			FLOAT,//小数
			BOOL,
			UNKNWON//未知
		};
		class XPathObject{
			//XPath的结果 再xpath.cpp实现
			public:
				XPathObject(_xmlXPathObject *obj,_xmlDoc *docptr = nullptr);
				//构造函数 后面参数是如doc是否被克隆了
				XPathObject(XPathObject &&);//转移函数
				XPathObject(const XPathObject &);//拷贝函数
				~XPathObject();
				NodeSet &operator *();
				NodeSet *operator ->();
				XPathObjectType type();//得到类型
				//转换函数
				std::string to_string();
				bool to_bool();
				double to_double();
				operator std::string();
				operator double();
				operator bool();
				Node &operator [](int val);//查找节点
				//得到节点数目
				unsigned int size();
				NodeSet &get_vec();//得到里面的vector
			private:
				XPathObjectType _type;//类型
				NodeSet vec;
				_xmlXPathObject *obj;
				_xmlDoc *docptr;//如果doc被克隆了 这个指针不为nullptr
		};
		class Node{
			//节点
			public:
				Node(_xmlNode *nodeptr,bool independence = true);
				//通过xmlNode新建节点
				Node(const char *name);
				Node(const char *name,const char *text);//文本节点
				Node(Node &&node);
				Node(const Node &);//拷贝
				~Node();
				long line();//得到行号
				bool is_blank();//是不是空白
				bool is_text();//是不是文本节点
				void unlink();//断开连接
				std::string operator [](const char *name);//查找attr
				std::string name();//得到名字
				std::string content();//这个节点的内容
				std::string text();//child->content
				std::string path();//得到节点的路径
				std::string to_string(int fmt = 1);//转化字符串
				//设置属性
				void set_name(const char *name);//设置名字
				void set_content(const char *content);//设置内容文本
				void add_content(const char *content);//添加文本
				void add_content(const char *content,int len);//添加文本 有长度的
				void add_attr(const char *name,const char *value);//添加一个特征
				bool has_attr(const char *name);//是否有这个特征
				bool remove_attr(const char *name);//移除一个特征
				//节点的关系得到
				Node prev() const;
				Node next() const;
				Node child() const;
				Node parent() const;
				//添加子节点
				void add_child(Node &&node);
				void add_child(Node &node);
				//添加前一个和后一个
				void add_next(Node &&node);
				void add_next(Node &node);
				//后一个
				void add_prev(Node &&node);
				void add_prev(Node &node);
				//复制
				Node copy();
				Node *clone();
				//计数
				//统计有几个孩子
				unsigned int count_child() const;
				unsigned int count_attr() const;//有几个特征
				_xmlNode *get_node() const;//得到节点
				//赋值
				Node &operator =(Node &&node);
				Node &operator =(Node &node);
				//XPath
				//在这个节点下面查找
				XPathObject xpath(const char *exp);
			private:
				_xmlNode *nodeptr;
				bool independence;
			friend class Doc;
		};
		class Doc{
			public:
				Doc(const char *version = default_version);
				Doc(const Doc &);//复制
				Doc(Doc &&doc);//转移构造
				Doc(_xmlDoc *docptr);
				~Doc();
				Node root();//得到根节点
				std::string to_string(int fmt = 1);//转换到字符串 默认格式化
				bool savefile(const char *file,int fmt = 1);//存到文件里面
				bool dump(FILE *fptr);//存储到文件里面
				void set_root(Node &&node);//设置根节点
				void set_root(Node &node);//设置根节点
				//XPath解析
				XPathObject xpath(const char *exp,bool clone = false);
				//第二个参数是否克隆自己 那样更安全 在Doc被销毁后
				static const char *default_version;
				static const char *default_encoding;
				static Doc LoadFile(const char *filename);//加载文件
				static Doc ParseString(const char *text,int len = 0);//解析字符串
			protected:
				_xmlDoc *docptr;
		};
		void ThrowLastError();//抛出最后一个错误
		void Init();//初始化
		void Quit();//退出
	};
	class HTML:public XML::Doc{
		public:
			//构建HTML
			HTML(const char *url,const char *ext_id);
			HTML(struct _xmlDoc *docptr);
			bool dump(FILE *fptr);
			bool savefile(const char *file,int fmt = 1);
			std::string to_string(int fmt = 1);//到字符串
			static HTML LoadFile(const char *filename);//加载文件
			static HTML ParseString(const char *text,int len = 0);
	};
};
#endif
