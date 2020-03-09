#ifndef _BOXNET_HPP_
#define _BOXNET_HPP_
namespace Box{
	namespace Net{
		//初始化和退出
		class Session;
		extern Session *global_session;
		void Init();
		void Quit();
		
	};
};
#endif
