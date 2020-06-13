#include <cstdio>
#include <cstdlib>
#include "stream.hpp"
namespace Box{
    Stream::~Stream(){
        ErrorCode code;
        close(&code);//关闭
    }
};