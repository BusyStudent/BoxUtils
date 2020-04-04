#ifndef _BOX_NET_SESSION_HPP_
#define _BOX_NET_SESSION_HPP_
#include <cstdio>
#include <string>
#include <functional>
namespace Box{
	namespace Net{
		//会话
		class EasyFactory;
		class EasyPackage;
		class Share;
		class Multi;
		class Session;
		typedef std::function <void(Session &,EasyPackage & pak,const std::string &content)> RequestCB;
		class RequestID{
			//请求ID
			public:
				RequestID(const RequestID &);//拷贝
				~RequestID();
				bool done();//设置完成 发送请求
				void set_finish_fn(RequestCB fn);
				void set_ostream(std::string &str);//设置输出流
				void set_ostream(FILE *f);//设置输出流
				void add_header(const char *key,const char *value);//加入头
				const char *operator [](const char *key) const;//查找请求头里面的值
			private:
				RequestID(Session *,RequestCB fn,EasyPackage *pak,bool osetd,bool doned);
				Session *session;//所属的会话
				RequestCB fn;//完成的 回调函数
				EasyPackage *pak;//请求包
				bool is_ostream_seted;//输出流设置过了
				bool is_done;//是否添加进去了
			friend class Session;
		};
		class Session{
			public:
				Session();
				Session(const Session &) = delete;
				~Session();
				//操控里面的工厂
				EasyFactory &operator ->();
				
				RequestID get(const char *url,RequestCB = nullptr);//发起Get请求
				//默认写到内存里
				void wait_all(int ms = 100);//等待所有完成
			private:
				void collect_multi_msg();//收集mutli的信息 调用fn
				Multi *multi;//Multi接口
				Share *share;//共享接口
				EasyFactory *factory;//对象工厂
			friend class RequestID;
		};
	};
};
#endif
