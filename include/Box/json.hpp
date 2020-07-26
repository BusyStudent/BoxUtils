#ifndef _BOX_JSON_HPP_
#define _BOX_JSON_HPP_
#include <cstddef>
#include <string>
#include <iosfwd>
#include <initializer_list>
#include "libc/attr.h"
#include "refptr.hpp"
struct cJSON;//替代了#include "cJSON.h"
namespace Box{
	struct JsonHolder{
		//Json的内部实现
		~JsonHolder();
		inline operator cJSON*() const noexcept;
		inline const cJSON *get() const noexcept;
		inline cJSON *get() noexcept;
		cJSON *cjson;//cJSON的数据
		bool ref;//是否是引用
	};
	class JsonIterator;
	class BOXAPI Json{
		public:
			//一些数据定义
			typedef JsonIterator iterator;
			typedef JsonIterator Iterator;
		public:
			//初始化是是否独立的
			Json(cJSON *item,bool ref = false);//是否是引用
			Json(Json &&json);//移动构造函数
			Json(const Json &);
			//从初始化列表构造
			Json(const std::initializer_list<int>&);
			Json(const std::initializer_list<double>&);
			Json(const std::initializer_list<const char*>&);
			~Json();
			//打印等调试
			void print() const;
			const char *get_type_string() const;//得到类型字符串
			char *to_cstring() const;//转化字符串
			std::string to_string() const;
			//类型判断
			bool is_null() const;//是null
			bool is_bool() const;
			bool is_true() const;//是真
			bool is_false() const;//是假
			bool is_string() const;//是否是字符串
			bool is_number() const;
			
			bool is_list() const;//是否是列表
			bool is_array() const;
			
			bool is_object() const;//是否是对象 或者说表
			bool is_table() const;
			
			//类型检查
			void check_is_array() const;//检查是数组
			void check_is_object() const;//检查是对象
			void check_is_number() const;//检查是数字
			void check_is_string() const;//检查是字符串
			void check_is_bool() const;//检查是bool
			
			void add_string(const char *str);
			void add_string(const char *key,const char *str);
			void add_number(double number);
			void add_number(const char *key,double number);
			
			void add_bool(bool val);//添加Bool值到数组里面
			void add_bool(const char *key,bool val);//到字典里面
			
			void add_null();//添加null数值到树组
			void add_null(const char *key);//到字典
			
			void add_item(Json &&item);//加入另一个节点到数组
			void add_item(Json &item);
			void add_item(const char *key,Json &&item);//加入对象里面 不就是字典么
			void add_item(const char *key,Json &item);
			//得到一些信息
			bool has(const char *key,bool case_sensitive = true) const;//是否有某个键
			//默认区分大小写
			int size() const;//直接得到大小(数组或者字典)
			int get_int() const;//得到int
			double get_number() const;//得到数字
			const char *get_string() const;//得到字符串
			const char *get_name() const;//如果在字典里面 得到名字
			//排序 默认大小写区分
			void sort(bool case_sensitive = true);
			void merge(const Json &json,bool case_sensitive = true);//合并
			//会检查name是否为nullptr
			//设置一些数值
			void set_int(const int &val);
			void set_number(const double &number);
			void set_string(const char *str);
			void set_string(const std::string &str);
			void set_bool(const bool &val);
			//迭代
			iterator begin();
			iterator end();
			
			Json operator [](const char*) const;//查找数据
			Json operator [](int val) const;
			
			Json pop(int val);//弹出在数组里面
			Json pop(const char *key,bool case_sensitive = true);//弹出在字典里面
			//默认大小写区分
			//比较Json
			bool cmp(const Json&,bool case_sensitive) const;//比较json 不过这个可以指定大小写敏感
			template <class T>
			inline bool operator !=(const T &t){
				//对于不等于的模板
				return not(operator==(t));
			}
			template <class T>
			explicit inline operator T(){
				//类型转换模板
				T var;
				operator >>(var);
				return var;
			}
			template <class T>
			inline void operator =(const T &t){
				//赋值转换的模板
				operator <<(t);
			}
			bool operator == (const Json &json) const;
			//一些比较对于里面的数据
			//比较字符串
			bool operator == (const std::string &s) const;
			bool operator == (const char *) const;
			bool operator == (int num) const;
			bool operator == (double num) const;
			bool operator == (bool val) const;
			bool operator == (std::nullptr_t ptr) const;//判断是否为nullptr
			void operator >> (int &i) const;//得到int
			void operator >> (double &number) const;//得到number
			void operator >> (char *&str) const;//得到复制过后的字符串
			void operator >> (std::string &str) const;
			void operator >> (const char * & str) const;//得到字符串
			void operator >> (bool &boolen) const;
			
			void operator << (const bool &boolen);
			void operator << (const int &i);
			void operator << (const double &number);
			void operator << (const std::string &str);
			void operator << (const char *str);
			//重载输出流
			cJSON *get_cjson();
			//一些操作
			Json copy() const;//复制一个在堆上
			Json *clone() const;//克隆一个Json
			//Json *move_toheap();//把数据转移到堆上
			//现在改成用std::move的
			//构造Json的类型
			static Json ParseString(const char *);
			static Json ParseString(const std::string &str);
			static Json ParseFrom(std::istream &stream);//从流解析
			static Json CreateNull();
			static Json CreateBool(bool boolen);
			static Json CreateTrue();
			static Json CreateFalse();
			static Json CreateTable();//创建一个表
			static Json CreateArray();//创建一个数组
			static Json CreateIntArray(int *array,int count);//创建一个Int的Array
			static Json CreateNumber(double number);
			static Json CreateString(const char *str);//创建一个字符串
			static Json CreateStringRef(const char *str);//引用字符串
			//格式化字符串
			static void MinifyString(char *str);
			//文件函数
			static void  SaveFile(Json &json,const char *filename);
			static Json  LoadFile(const char *filename);
			//得到内部解析器的错误
			static const char *GetError();//这个线程不安全的
			static const char *Version();//得到版本
			//cJSON的malloc和free
			static void *Malloc(size_t size);
			static void  Free(void *ptr);
			friend std::ostream& operator <<(std::ostream&,const Json&);
		private:
			RefPtr<JsonHolder> holder;//包裹Json的实现
		friend class JsonIterator;
	};
	//迭代器的实现
	class BOXAPI JsonIterator{
		public:
			JsonIterator(cJSON *cjson,cJSON *prev = nullptr);
			~JsonIterator();
			inline Json &operator *(){
				return json;
			}
			inline Json *operator ->(){
				return &json;
			}
			inline const Json &operator *() const{
				return json;
			}
			inline const Json *operator ->() const{
				return &json;
			}
			//一些解操作
			//移动迭代器操作
			void operator ++();
			void operator ++(int);
			void operator --();
			void operator --(int);
			bool operator == (const JsonIterator &);
			bool operator != (const JsonIterator &);
		private:
			cJSON *prev;//前面一个
			Json json;
	};
};
#endif
