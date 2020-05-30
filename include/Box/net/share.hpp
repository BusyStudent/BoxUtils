#ifndef _BOX_NET_SHARE_HPP_
#define _BOX_NET_SHARE_HPP_
namespace Box{
	namespace Net{
		//共享接口
		class Easy;
		class EasyPackage;
		class Share{
			public:
				Share();
				Share(const Share &) = delete;
				//禁止拷贝
				~Share();
				//一些方法
				void set_share_cookie();//设置共享Cookie
				void set_share_connect();//共享连接
				void set_share_dns();//共享DNS
				void set_share_psl();//共享公共后缀
				void set_share_ssl_session();//共享SSL会话ID
				void set_share_all();//分享上面所有的
				
				void *get_handle() const;//得到曲柄
				//加入 如Easy或者EasyPackage
				
				void add_handle(Easy & easy);//加入曲柄
				void add_handle(Easy * easy);//加入曲柄指针
				void add_handle(EasyPackage &pak);//加入包
				void add_handle(EasyPackage *pak);//加入包指针
				//移除
				void remove_handle(Easy &easy);//移除曲柄
				void remove_handle(Easy *easy);//移除曲柄指针
				void remove_handle(EasyPackage &pak);//移除包
				void remove_handle(EasyPackage *pak);//移除包指针
			private:
				struct Mutexs;//内部锁集合
				void *handle;
				Mutexs *mutexs;//狐池锁的结构体指针
		};
	};
};
#endif
