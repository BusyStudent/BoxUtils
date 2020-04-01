#ifndef _BOX_UI_HPP_
#define _BOX_UI_HPP_
namespace Box{
	namespace UI{
		void Init();//初始化
		void Quit();//退出 一般不用调用这个
		void Panic(const char *msg,...);//错误退出
		void LogError(const char *msg,...);//输出错误
		int  Main();//进入主事件循环
	};
};
#endif
