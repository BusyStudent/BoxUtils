#include "net/easy.hpp"
#include "net/multi.hpp"
#include "net/share.hpp"
#include "net/factory.hpp"
#include "net/session.hpp"
namespace Box{
namespace Net{
	struct ReqInfo{
		//请求详情
		inline ~ReqInfo(){
			delete pak;
		}
		RequestCB fn;//回调函数
		EasyPackage *pak;//包
		std::string content;//内容
		void *userdata;//用户数据
	};
	Session::Session(){
		//共享所有
		share = new Share;
		multi = new Multi;
		factory = new EasyFactory;
		//生成一些要得东西
		share->set_share_all();
		(*factory)->enable_cookie();//启动Cookie引擎
	}
	Session::~Session(){
		//销毁掉
		delete factory;
		delete share;
		delete multi;
	}
	EasyFactory &Session::operator ->(){
		return *factory;
	}
	//收集信息
	void Session::collect_multi_msg(){
		multi->for_msg([this](MultiMsg &msg) -> void {
			msg.multi->remove_handle(msg.easy);
			//从mutli移除曲柄
			ReqInfo *info = (ReqInfo *)msg.userdata;
			//得到请求信息
			if(info != nullptr){
				if(info->fn != nullptr){
					//有回调函数 调用
					info->fn(*this,*(info->pak),info->content,info->userdata);
				}
			}
			delete info;
		});
	}
	//等待所有完成
	void Session::wait_all(int ms){
		int running_handle;
		do{
			multi->perform(running_handle);
			//执行
			collect_multi_msg();//收集信息
			multi->wait(ms);//等待
		}
		while(running_handle);
	}

	RequestID Session::get(const char *url,RequestCB  fn){
		EasyPackage *pak = factory->allocate(url);
		//申请包
		return RequestID(this,fn,pak,false,false);
	}

	//RequestID得实现
	RequestID::RequestID(Session *s,RequestCB f,EasyPackage *p,bool osetd,bool doned){
		session = s;
		fn = f;
		pak = p;
		is_ostream_seted = osetd;
		is_done = doned;
		userdata = nullptr;
	}
	//拷贝函数
	RequestID::RequestID(const RequestID &req){
		pak = new EasyPackage(*(req.pak));
		fn = req.fn;
		session = req.session;
		is_ostream_seted = req.is_ostream_seted;
		is_done = req.is_done;
		if(is_done == true){
			//如果已经发送了不拷贝请求包 直接给个指针
			pak = req.pak;
		}
		else{
			pak = new EasyPackage(*(req.pak));
		}
		userdata = req.userdata;
	}
	RequestID::~RequestID(){
		if(is_done == false){
			//没有发送出去 回收资源
			delete pak;
		}
	}
	bool RequestID::done(){
		//如果已经加入了
		if(is_done == true){
			return false;
		}
		ReqInfo *info = new ReqInfo();
		//加入信息
		if(is_ostream_seted == false){
			//没有设置输出流
			(*pak)->set_ostream(info->content);
		}
		//设置详情的内容
		info->pak = pak;
		//请求包
		info->fn = fn;
		//回调函数
		info->userdata = userdata;
		session->multi->add_handle(pak,info);
		//session->share->add_handle(pak);
		//加入曲柄
		int r;
		session->multi->perform(r);//执行
		is_done = true;
		return true;
	}
	//设置用户数据
	void RequestID::set_userdata(void *data){
		userdata = data;
	}
	void RequestID::set_finish_fn(RequestCB fn){
		this->fn = fn;
	}
	//设置输出流
	void RequestID::set_ostream(std::fstream &stream){
		//设置回调
		(*pak)->set_write_cb(
			[](char *buf,size_t num,size_t block,void *stream) -> size_t{
				auto s = ((std::fstream*)stream);
				s->write(buf,num * block);
				return num*block;
			},
			&stream
		);
	}
	void RequestID::set_ostream(std::string &str){
		is_ostream_seted = true;
		(*pak)->set_ostream(str);
	}
	void RequestID::set_ostream(FILE *f){
		is_ostream_seted = true;
		(*pak)->set_ostream(f);
	}
	//加入请求头
	void RequestID::add_header(const char *key,const char *value){
		pak->_headers.add(key,value);
	}
	//查找请求头里面的值
	const char *RequestID::operator [](const char*key) const{
		return pak->_headers[key];
	}
}
}