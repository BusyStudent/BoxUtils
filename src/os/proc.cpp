#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#include "common/def.hpp"
#include "os/proc.hpp"
namespace Box{
namespace OS{
    bool Daemon(){
        #ifdef _WIN32
        return FreeConsole() == TRUE;
        #else
        //不重定向当前工作目录 分离控制台
        return daemon(1,0) == 0;
        #endif
    }
};
};