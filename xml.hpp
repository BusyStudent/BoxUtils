#ifndef _BOX_XML_HPP_
#define _BOX_XML_HPP_
#include <string>
#include <functional>
namespace Box{
	namespace XML{
		class Node;
		namespace _Utils{
			//一些工具
			void *CopyNodePtr(Node &);//复制节点
			/*
			 * 如果节点是独立的 那修改为不独立 返回nodeptr
			 * 否则复制nodeptr
			 */
		};
		class Attr{
			//特征
			public:
				Attr(void *attrptr,bool independence = true);
				Attr(const Attr&);
				~Attr();
				const char *get_name();//得到名字
				const char *get_content();//得到内容
			private:
				void *attrptr;
				bool independence;
			friend class Node;
		};
		class Node{
			//XML节点
			public:
				Node(void *nodeptr,bool independence = true);
				//从指针生成一个 默认是独立的
				~Node();
				//结构操作
				void add_text_child(const char *name,const char *content);
				void add_child(Node &child);//添加子节点
				//信息获取
				const char *get_content();//得到内容
				const char *get_name();//得到名字
				unsigned int get_line();//得到行数
				//Attr
				void add_attr(const char *name,const char *content);//加入特征
				bool has_attr(const char *name);//是否有属性
				Attr get_attr(const char *name);//得到特征通过名字
				void for_attr(std::function <bool(Attr &)>);//遍历attr
				std::string get_attr_content(const char *name);//得到attr的内容
				std::string operator [](const char *name);//查找attr
				//设置信息
				void set_content(const char *str);
				//遍历
				void for_each(std::function <bool(Node&)>);//遍历
				//创建函数
				
				static Node Create(const char *name);//创建节点
				static Node CreateText(const char *content);//创建文本
			private:
				bool independence;//是否独立
				void *nodeptr;
			friend class Doc;
			friend void *_Utils::CopyNodePtr(Node&);
		};
		typedef Node Elem;//元素
		class Doc{
			//文档
			public:
				Doc(void *docptr);//通过DocPtr初始化
				Doc(const Doc&);//拷贝
				~Doc();
				//一些函数
				void set_root(Node &root);//设置根节点
				//得到内容
				Node get_root();//得到根节点
				const char *get_version();//得到版本
				const char *get_encoding();//得到编码信息
				
				std::string to_string(int format = 1);//到字符串
				void save_file(const char *filename,int format = 1);//保存
				//format表示是否格式化
				//默认版本
				void *get_docptr();//得到docptr指针
				static const char *default_version;
				static const char *default_encoding;//默认编码
				static Doc Create(const char *version = default_version);//创建一个文档
				static Doc LoadFile(const char *filename);//加载文件
				static Doc ParseString(const char *str);//解析字符串
			protected:
				//libxml doc的指针
				void *docptr;
		};
		void InitParser();//初始化Parser
		void CleanupParser();//释放解析器
		void MemoryDump();//释放内存
		void Cleanup();//总体回收内存
	};
	class HTML:public XML::Doc{
		public:
			typedef XML::Node Node;
			std::string to_string(int format = 1);//到字符串
			void save_file(const char *filename,int format = 1);//保存
			static HTML LoadFile(const char *filename,
				const char *encoding = default_encoding);//加载文件
			static HTML ParseString(const char *str,
				const char *url = nullptr,
				const char *encoding = default_encoding);//解析字符串
		private:
			HTML(void *);
	};
};
#endif
