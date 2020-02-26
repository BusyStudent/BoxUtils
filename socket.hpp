#ifndef _BOXUTILS_SOCKET_HPP_
#define _BOXUTILS_SOCKET_HPP_
//Socket部分
#ifdef _WIN32
	#include <winsock.h>
#else
	#include <stdio.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
#endif
#include <string>
namespace BoxUtils{
	namespace SocketV4{
		class Address{
			public:
				Address(){};
				Address(const char *ip,unsigned int port);
				Address(struct sockaddr_in &addr);
				std::string to_string();
				std::string ip;
				unsigned int port;
			private:
				void load_struct(struct sockaddr_in &addr);
				struct sockaddr_in addr;
			friend class Socket;
			friend class TCP;
		};
		class Socket{
			public:
				Socket(){};
				Socket(int fd);
				virtual ~Socket();
				void connect(Address &);
				void bind(Address &);
				void bind(const char *ip,unsigned int port);
				void listen(int backlog);
				void close();
				ssize_t write(const void *buf,size_t len);
				ssize_t read(void *buf,size_t len);
				ssize_t operator << (std::string &str);
			protected:
				int fd;
		};
		class TCP:public Socket{
			public:
				TCP *accept(Address *addr);
				TCP();
		};
		class UDP:public Socket{

		};
	};
};
#endif
