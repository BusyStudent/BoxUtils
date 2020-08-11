#include <curl/curl.h>
#include <curl/curlver.h>
#include <string>
#include <cstring>
#include <ostream>
#include <chrono>

#include "common/def.hpp"

#include "exception.hpp"
#include "net/exception.hpp"
#include "net/headers.hpp"
#include "net/easy.hpp"
#include "net.hpp"
namespace Box{
namespace Net{
	//检查EASY的返回代码
	#define CURL_ASSERT(CODE) {CURLcode _code_ = CODE;\
		if(_code_ != CURLE_OK){\
		throw Box::Net::EasyException(_code_);\
	}}
	Easy::Easy(void *hd){
		handle = hd;
		//设置下自己本身
		curl_easy_setopt(handle,CURLOPT_PRIVATE,this);
		multi_userdata = nullptr;
	}
	Easy::Easy(){
		handle = curl_easy_init();
		//设置为自己本身
		curl_easy_setopt(handle,CURLOPT_PRIVATE,this);
		#ifndef NDEBUG
		curl_easy_setopt(handle,CURLOPT_VERBOSE,1L);
		#endif
		//生成一个Handle
		#ifndef _WIN32
		curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//不要有信号
		#endif
		//清空本身数据
		multi_userdata = nullptr;
	}
	//移动一下
	Easy::Easy(Easy &&easy){
		handle = easy.handle;
		multi_userdata = easy.multi_userdata;
		curl_easy_setopt(handle,CURLOPT_PRIVATE,this);//重新设置一下自身
		//清空一下
		easy.handle = nullptr;
		easy.multi_userdata = nullptr;
	}
	Easy::Easy(const Easy &easy){
		//复制一个
		handle = curl_easy_duphandle(easy.handle);
		curl_easy_setopt(handle,CURLOPT_PRIVATE,this);
		multi_userdata = nullptr;
	}
	Easy::~Easy(){
		curl_easy_cleanup(handle);
	}
	void Easy::reset(){
		curl_easy_reset(handle);
		#ifndef _WIN32
		curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);//不要有信号
		#endif
		curl_easy_setopt(handle,CURLOPT_PRIVATE,this);//设置自身
		multi_userdata = nullptr;
	}
	void Easy::perform(){
		//进行传输
		CURL_ASSERT(curl_easy_perform(handle));
	}

	//启动cookie引擎
	void Easy::enable_cookie(){
		curl_easy_setopt(handle,CURLOPT_COOKIEFILE,"");
	}
	//添加cookie
	void Easy::add_cookie(const char *cookie){
		curl_easy_setopt(handle,CURLOPT_COOKIELIST,cookie);
	}
	//设置cookie
	void Easy::set_cookie(const char *cookie){
		curl_easy_setopt(handle,CURLOPT_COOKIE,cookie);
	}

	void Easy::set_url(const char *url){
		//设置URL
		curl_easy_setopt(handle,CURLOPT_URL,url);
	}
	void Easy::set_url(const std::string &url){
		//设置URL
		curl_easy_setopt(handle,CURLOPT_URL,url.c_str());
	}
	void Easy::set_method(Method method){
		//设置方法
		switch(method){
			case Method::POST:{
				curl_easy_setopt(handle,CURLOPT_POST,1L);
				break;
			}
			case Method::GET:{
				curl_easy_setopt(handle,CURLOPT_HTTPGET,1L);
				break;
			}
			case Method::PUT:{
				curl_easy_setopt(handle,CURLOPT_PUT,1L);
				break;
			}
		}
	}
	void Easy::set_timeout(long timeout_ms){
		//设置超时
		curl_easy_setopt(handle,CURLOPT_CONNECTTIMEOUT_MS,timeout_ms);
	}
	void Easy::set_timeout(const std::chrono::milliseconds &ms){
		set_timeout(ms.count());
	}
	void Easy::set_proxy(const char *proxy){
		curl_easy_setopt(handle,CURLOPT_PROXY,proxy);
	}
	void Easy::set_useragent(const char *str){
		//设置User-Agent
		curl_easy_setopt(handle,CURLOPT_USERAGENT,str);
	}
	void Easy::set_referer(const char *str){
		//设置referer
		curl_easy_setopt(handle,CURLOPT_REFERER,str);
	}
	void Easy::set_headers(){
		//重置会原有的请求头
		curl_easy_setopt(handle,CURLOPT_HTTPHEADER,nullptr);
	}
	void Easy::set_headers(const Headers & h){
		//使用请求头替换里面的头部
		curl_easy_setopt(handle,CURLOPT_HTTPHEADER,h.slist);
	}
	void Easy::set_ostream(std::string &str){
		//设置输出字符串
		curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,WriteToString);
		curl_easy_setopt(handle,CURLOPT_WRITEDATA,&str);
	}
	void Easy::set_ostream(FILE *f){
		//设置输出流
		curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,WriteToFILE);
		curl_easy_setopt(handle,CURLOPT_WRITEDATA,f);
	}
	//设置输出流到std::ostream
	void Easy::set_ostream(std::ostream &stream){
		set_write_cb(
			[](char *buf,size_t num,size_t blk,void *param) -> size_t{
				auto stream = static_cast<std::ostream*>(param);
				stream->write(buf,blk * num);
				if(not stream->bad()){
					//如果没错误
					return blk * num;
				}
				return 0;
			},&stream
		);
	}
	void Easy::set_oheaders(Headers &h){
		//设置输出的响应头
		curl_easy_setopt(handle,CURLOPT_HEADERFUNCTION,WriteToHeaders);
		curl_easy_setopt(handle,CURLOPT_HEADERDATA,&h);
	}
	void Easy::set_followlocation(){
		//自动更寻重定向
		curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1L);
		curl_easy_setopt(handle,CURLOPT_AUTOREFERER,1L);
	}
	void Easy::set_write_cb(EasyCallBack cb,void *param){
		//写的CALLBACK
		curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,cb);
		curl_easy_setopt(handle,CURLOPT_WRITEDATA,param);
	}
	void Easy::set_header_cb(EasyCallBack cb,void *param){
		//头的callback
		curl_easy_setopt(handle,CURLOPT_HEADERFUNCTION,cb);
		curl_easy_setopt(handle,CURLOPT_HEADERDATA,param);
	}
	//Post数据设置
	void Easy::set_post(const void *data,long datasize,bool copy){
		set_method(Method::POST);//设置Post模式
		CURLcode code;
		//设置大小
		curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,datasize);
		if(copy){
			//如果要复制数据
			code = curl_easy_setopt(handle,CURLOPT_COPYPOSTFIELDS,data);
		}
		else{
			code = curl_easy_setopt(handle,CURLOPT_POSTFIELDS,data);
		}
		if(code != CURLE_OK){
			//失败
			throw EasyException(code);
		}
	}
	//Post字符串数据
	void Easy::set_post(const std::string &str){
		return set_post(str.c_str(),str.length() * sizeof(char),true);
	}
	//POST表单
	void Easy::set_post(const Mime &mime){
		curl_easy_setopt(handle,CURLOPT_MIMEPOST,mime.mime);
	}
	//清空所有cookie
	void Easy::clear_cookie(){
		curl_easy_setopt(handle,CURLOPT_COOKIELIST,"ALL");
	}
	//清空会话cookie
	void Easy::reset_cookie(){
		curl_easy_setopt(handle,CURLOPT_COOKIELIST,"SESS");
	}

	void *Easy::get_handle() const{
		//得到CURL的Handle
		return handle;
	}
	long Easy::status_code()const{
		//得到状态代码
		long code;
		CURL_ASSERT(curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,&code));
		return code;
	}
	std::string Easy::url() const{
		//得到URL
		#if LIBCURL_VERSION_MAJOR == 7 && LIBCURL_VERSION_MINOR >= 52
			const char *url = nullptr;
			CURL_ASSERT(curl_easy_getinfo(handle,CURLINFO_EFFECTIVE_URL,&url));
			return std::string(url);
		#else
			throw std::exception();
		#endif
	}
	std::string Easy::content_type() const{
		//类型
		char *type = nullptr;
		CURL_ASSERT(curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE,&type));
		return std::string(type);
	}
	std::string Easy::scheme() const{
		char *scheme = nullptr;
		CURL_ASSERT(curl_easy_getinfo(handle,CURLINFO_SCHEME,&scheme));
		return scheme;
	}
	bool Easy::ok()const{
		if(status_code() == 200){
			return true;
		}
		else{
			return false;
		}
	}
	//克隆
	Easy *Easy::clone()const{
		return new Easy(curl_easy_duphandle(handle));
	}
	//URL编码和解码

	std::string Easy::escape_url(const char *url) const{
		char *s = curl_easy_escape(handle,url,0);
		//编码一下
		if(s == nullptr){
			throw Box::NullPtrException();
		}
		//失败
		std::string str(s);
		curl_free(s);
		return str;
	}
	std::string Easy::unescape_url(const char *url) const{
		//解码URL
		char *s = curl_easy_escape(handle,url,0);
		if(s == nullptr){
			throw Box::NullPtrException();
		}
		std::string str(s);
		curl_free(s);
		return str;
	}
	void Easy::throw_for_status(){
		auto code = status_code();
		if(code != 200){
			throw HttpError(code);
		}
	}

	//回调函数
	size_t Easy::WriteToFILE(char *buf,size_t size,size_t block,void *param){
		//写到文件
		return fwrite(buf,size,block,(FILE*)param);
	}
	size_t Easy::WriteToString(char *buf,size_t size,size_t block,void *param){
		//写到字符串里
		size_t s = size*block;
		((std::string*)param)->append(buf,s);
		return s;
	}
	size_t Easy::WriteToHeaders(char *buf,size_t size,size_t block,void *param){
		std::string strbuf(buf,block);
		if(strbuf.find(':') == strbuf.npos){
			//没找到:
			//跳过
			return size * block;
		}
		#if 1
		//这里处理末尾的\r\n
		std::string::iterator iter = --strbuf.end();
		while(*iter == '\r' or *iter == '\n'){
			//取出\r\n
			strbuf.pop_back();
			iter = --strbuf.end();
		}
		#endif
		//直接加入
		//strbuf.replace(strbuf.begin(),strbuf.end(),"\r\n","");
		((Headers*)param)->add_string(strbuf.c_str());
		return size*block;
	}
	//得到引用
	Easy &Easy::GetRefFrom(void *handle){
		Easy *ref = nullptr;
		curl_easy_getinfo(handle,CURLINFO_PRIVATE,&ref);
		if(ref == nullptr){
			throw Box::NullPtrException();
		}
		return *ref;
	}
	//表单实现
	Mime::Mime(const Easy &easy){
		//创建一下
		mime = curl_mime_init(easy.handle);
	}
	Mime::~Mime(){
		curl_mime_free(mime);
	}
	MimePart Mime::addpart(){
		//添加一个部分
		return {
			.part = curl_mime_addpart(mime)
		};
	}
	//表单的部分
	void MimePart::set_headers(){
		//清空
		CURL_ASSERT(curl_mime_headers(part,nullptr,0));
	}
	void MimePart::set_headers(const Headers &headers,bool copy){
		//设置请求头
		CURLcode code;
		if(copy){
			//要复制一下
			code = curl_mime_headers(part,SListDup(headers.slist),1);
		}
		else{
			code = curl_mime_headers(part,headers.slist,0);
		}
		CURL_ASSERT(code);
	}
	void MimePart::set_encoder(const char *encoder){
		//更改编码器
		CURL_ASSERT(curl_mime_name(part,encoder));
	}
	void MimePart::set_name(const char *name){
		//设置名字
		CURL_ASSERT(curl_mime_name(part,name));
	}
	void MimePart::set_data(const void *data,size_t datasize){
		//设置数据从内存中
		CURL_ASSERT(curl_mime_data(part,(const char*)data,datasize));
	}
	void MimePart::set_filedata(const char *filename){
		//设置数据从文件中
		CURL_ASSERT(curl_mime_filedata(part,filename));
	}
	void MimePart::set_type(const char *type){
		//设置类型
		CURL_ASSERT(curl_mime_type(part,type));
	}
}
}
