#ifndef _BOX_XV_HPP_
#define _BOX_XV_HPP_
#include <functional>
#include <list>
#include <string>
#include "buffer.hpp"
#include "socket.hpp"
//仿照Python SocketServer写的东西
namespace Box{
	namespace Xv{
		struct Event{
			//事件
		};
		struct ServiceImpl;
		class Service{
			//服务
			public:
				Service(unsigned int workers = 4);//有几个线程
				~Service();
				void loop();//运行
			private:
				ServiceImpl *service;
			protected:

		};
	};
};
#endif
