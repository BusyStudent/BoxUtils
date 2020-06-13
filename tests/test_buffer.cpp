//这个Buffer没写好
#include <Box/buffer.hpp>
#include <cstring>
int main(){
    using Box::TBuffer;
    TBuffer<char> buf(2);
    buf.append("Hello World",strlen("Hello World"));
    
}