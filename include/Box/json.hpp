#ifndef _BOX_JSON_HPP_
#define _BOX_JSON_HPP_
#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>
#include <initializer_list>
#include "libc/attr.h"
struct cJSON;//替代了#include "cJSON.h"
namespace Box{
	class Json;
	class JsonIterator;
	//Cjson的引用 自身一个引用
	class BOXAPI JsonRef{
		public:
			//Helper 来构建Json辅助类型
			struct BOXAPI Helper{
				Helper(int);
				Helper(bool);
				Helper(float);
				Helper(double);
				Helper(const char *);
				Helper(const std::string&);
				Helper(std::string_view);
				Helper(std::initializer_list<Helper>);
				cJSON *value;
			};
		public:
			//转换一下
			JsonRef():value(nullptr){}
			JsonRef(cJSON *v):value(v){}
			//到字符串
			std::string to_string(bool formated = true) const;
			//得到数据
			template<class T>
			T get_value() const;
			//设置数据
			void set_value(int);
			void set_value(bool);
			void set_value(float);
			void set_value(double);
			void set_value(JsonRef);
			void set_value(Json &&);
			void set_value(const Helper &);
			void set_value(std::string_view);
			//得到类型
			std::string_view type() const noexcept;
			//得到大小
			int size() const;
			//得到名字
			std::string_view name() const;
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
			//插入数据
			//到数组
			void insert(Json &&);//这个直接移动进去
			void insert(JsonRef);//这个会拷贝一份
			void insert(const Helper &);
			//到数组或者列表
			void insert(int index,Json &&);
			void insert(int index,JsonRef);
			void insert(int index,const Helper &);
			//到表
			void insert(std::string_view,Json&&);
			void insert(std::string_view,JsonRef);
			void insert(std::string_view,const Helper &);
			//是否有那个键值
			bool has(std::string_view,bool case_sensitive = true) const;
			bool has(int index) const;
			Json pop(std::string_view);
			Json pop(int index);
			//移除所有的项
			void clear();
			//是否有数值
			bool has_value() const noexcept{
				return value != nullptr;
			}
			//得到path 
			std::string get_path(JsonRef) const;
			//迭代器
			JsonIterator find(JsonRef ref);//查找数值
			JsonIterator erase(JsonIterator iter);//移除一个项
			JsonIterator begin();
			JsonIterator   end();
			//比较 是否对大小写敏感
			bool cmp(JsonRef ref,bool case_sensitive = true);
			//比较的模板
			template<class T>
			bool cmp(T &&data) const noexcept{
				return JsonRef::operator ==(data);
			};
			//operator
			template<class T>
			operator T() const{
				return get_value<T>();
			};
			//查找数据
			JsonRef operator [](int index) const;
			JsonRef operator [](std::string_view) const;
			JsonRef operator [](const char *str) const{
				return JsonRef::operator [](std::string_view(str));
			}
			//比较
			bool operator ==(int) const noexcept;
			bool operator ==(bool) const noexcept;
			bool operator ==(float) const noexcept;
			bool operator ==(double) const noexcept;
			bool operator ==(JsonRef) const noexcept;
			bool operator ==(std::nullptr_t) const noexcept;
			bool operator ==(std::string_view) const noexcept;
			bool operator ==(const Json & json) const noexcept;
			bool operator ==(const char * str) const noexcept{
				return JsonRef::operator ==(std::string_view(str));
			}
			//反向比较
			template<class T>
			bool operator !=(const T &data) const noexcept{
				return not JsonRef::operator ==(data);
			};
			//设置数据
			template<class T>
			JsonRef &operator =(T &&data){
				JsonRef::set_value(std::forward<T>(data));
				return *this;
			}
			//为了可以为这样 json["a"] = {41,"a"};
			JsonRef &operator =(Helper &&helper){
				JsonRef::set_value(std::forward<Helper&&>(helper));
				return *this;
			}
			//ostream输出函数
			friend std::ostream & operator <<(std::ostream&,JsonRef);
		private:
			cJSON *value;
		friend class JsonIterator;
		friend class Json;
	};
	class BOXAPI Json:public JsonRef{
		public:
			//一些数据定义
			typedef JsonIterator iterator;
			typedef JsonIterator Iterator;
			//Helper
			struct Helper{
				cJSON *value;
			};
		public:
			//构造器
			Json():JsonRef(nullptr),refcount(nullptr){};//空的数值
			Json(cJSON *cjs);
			//浅复制
			Json(const Json &json);
			Json(Json &&json);
			//创建数据
			Json(const Helper &);//从Helper构建
			~Json();
			// //转换到引用
			// operator JsonRef() const noexcept{
			// 	return value;
			// }
			Json clone() const;//克隆 深拷贝
			static Json ParseString(std::string_view);
			static Json ParseFrom(std::istream &stream);//从流解析
			static Json CreateNull();
			static Json CreateBool(bool boolen);
			static Json CreateTrue();
			static Json CreateFalse();
			static Json CreateTable();//创建一个表
			static Json CreateArray();//创建一个数组
			static Json CreateIntArray(int *array,int count);//创建一个Int的Array
			static Json CreateNumber(double number);
			static Json CreateString(std::string_view);//创建一个字符串
			static Json CreateStringRef(std::string_view);//引用字符串
			//格式化字符串
			static void MinifyString(char *str);
			static void MinifyString(std::string &);
			//文件函数
			static void  SaveFile(Json &json,std::string_view);
			static Json  LoadFile(std::string_view);
			//得到内部解析器的错误
			static const char *GetError();
			/*在GCC 和 MSVC上是线程安全的 其他的就不知道了*/
			static const char *Version();//得到版本
			//cJSON的malloc和free
			static void *malloc(size_t size);
			static void  free(void *ptr);
		protected:
			void unref();//减少引用
			void ref();//增加引用
		private:
			//引用计数器
			struct RefCount{
				int count;
			};
			RefCount *refcount;
		friend class JsonIterator;
		friend class JsonRef;

		friend struct JsonRef::Helper;
	};
	//迭代器的实现
	class BOXAPI JsonIterator{
		public:
			//初始化迭代器
			JsonIterator():prev(nullptr),json(nullptr){};
			JsonIterator(JsonRef ref);
			JsonIterator(cJSON *cjson,cJSON *prev = nullptr)
				:prev(prev),json(cjson){};
			//一些解操作
			JsonRef &operator *(){
				return json;
			}
			JsonRef *operator ->(){
				return &json;
			}
			const JsonRef &operator *() const{
				return json;
			}
			const JsonRef *operator ->() const{
				return &json;
			}
			//是否所指的地方无效 JsonRef::end() 要遍历到尾部 太慢
			bool bad() const noexcept{
				return json.value == nullptr;
			}
			std::string_view key() const;//得到键值
			//移动迭代器操作
			void operator ++();
			void operator --();
			JsonIterator operator ++(int); 
			JsonIterator operator --(int); 
			bool operator == (const JsonIterator &iter) const noexcept{
				return json.value == iter.json.value;
			};
			bool operator != (const JsonIterator &iter) const noexcept{
				return json.value != iter.json.value;
			};
		private:
			cJSON *prev;//前面一个
			JsonRef json;
	};
	//Format
	namespace Fmt{
		inline std::string ToString(const JsonRef &ref){
			return ref.to_string();
		}
		inline std::string ToString(const Json &json){
			return json.to_string();
		}
	};
	//JsonRef转换的函数
	//转换到int
	template<>
	int    JsonRef::get_value<int>() const;
	//转换到bool
	template<>
	bool   JsonRef::get_value<bool>() const;
	//转换到float
	template<>
	float  JsonRef::get_value<float>() const;
	//转换到double
	template<>
	double JsonRef::get_value<double>() const;
	//转换到string
	template<>
	std::string JsonRef::get_value<std::string>() const;
	//转换到string_view
	template<>
	std::string_view JsonRef::get_value<std::string_view>() const;
	//转换到const char *
	template<>
	const char *JsonRef::get_value<const char*>() const;
	//得到cjson
	template<>
	inline cJSON *JsonRef::get_value<cJSON*>() const{
		return value;
	}
};
#endif
