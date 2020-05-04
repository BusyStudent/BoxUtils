#include "net_headers.hpp"
#include "net_factory.hpp"
using namespace Box::Net;
EasyPackage::EasyPackage(Easy *e,const Headers &h){
	//产出的对象
	easy_handle = e;
	_headers.update(h);
	easy_handle -> set_oheaders(resp_headers);//设置输出的头
	easy_handle -> set_headers(_headers);//设置自己的头部为请求头
}
EasyPackage::EasyPackage(const EasyPackage &pak){
	//拷贝函数
	easy_handle = pak.easy_handle->clone();
	//克隆一个handle
	_headers.update(pak._headers);
	//拷贝headers
	easy_handle -> set_oheaders(resp_headers);
	easy_handle -> set_headers(_headers);
}
EasyPackage::~EasyPackage(){
	//销毁Easy Handle
	delete easy_handle;
}
void EasyPackage::for_content(std::function <void(std::string&)> fn){
	easy_handle->set_write_cb([](char *buf,size_t block,size_t numb,void* fn)->size_t{
		//设置回调
		std::string strbuf(buf,numb*block);//把所有内容复制进来
		(*(std::function <void(std::string&)>*)fn)(strbuf);//调用函数
		return numb*block;
	},(void*)(&fn));
	easy_handle->perform();
}
std::string EasyPackage::content(){
	//得到数据
	std::string content;
	easy_handle->set_ostream(content);
	easy_handle->perform();
	return content;
}
//Factory
EasyPackage EasyFactory::create(const char *url){
	//克隆一个模板
	auto e = _easy.clone();
	e->set_url(url);//设置URL
	return EasyPackage(e,_headers);
}
EasyPackage *EasyFactory::allocate(const char *url){
	//在栈上
	auto e = _easy.clone();
	e->set_url(url);//设置URL
	return new EasyPackage(e,_headers);
}
Headers &EasyFactory::headers(){
	return _headers;
}
void EasyFactory::add_header(const char *key,const char *value){
	_headers.add(key,value);
}
