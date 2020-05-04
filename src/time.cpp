#include <ctime>
#include "time.hpp"
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
using namespace Box;
void Time::USleep(int us){
	#ifdef _WIN32
	Sleep(us);
	#else
	usleep(us);
	#endif
}
