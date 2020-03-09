#ifndef _BOXNET_MAIN_HPP_
#define _BOXNET_MAIN_HPP_
//主要的请求部分
#include <string>
#include "net_table.hpp"
namespace Box{
	class TCPSocket;
	namespace Net{
		class Session;
		namespace Method{
			//请求类型
			enum T{
				POST,
				GET
			};
		};
		class Request{
			public:
				Request(Method::T m,const char *url,
					Headers *headers = nullptr,bool stream = false);
				//公共变量
				std::string format();//格式化自己
			private:
				std::string schema;//协议模式如HTTP HTTPS
				std::string path;//请求的部分
				std::string hostname;//域名
				Headers headers;
				bool stream;
				Method::T method;//请求方式
				uint16_t port;
			friend class Session;
		};
		class Response{
			//响应
			public:
				~Response();
				std::string &content();//获取内容
				static ssize_t HTTPSendData(Response&,const void *buf,size_t len);
				static ssize_t HTTPRecvData(Response&,void *buf,size_t len);
				
				ssize_t send(const void *buf,size_t len);
				ssize_t recv(void *buf,size_t len);
				bool connect(std::string &ip,uint16_t port);
			private:
				ssize_t (*send_data_func)(Response&,const void *buf,size_t len);//发送数据
				ssize_t (*recv_data_func)(Response&,void *buf,size_t len);
				bool (*connect_func)(Response&,std::string &ip,uint16_t port);
				Response(Session *session,TCPSocket *sock,bool stream);
				TCPSocket *sock;
				std::string hostname;
				std::string _content;//内容
				Session *session;//所属回话
				bool stream;
				int timeout;
			friend class Session;
		};
	};
};
#endif
