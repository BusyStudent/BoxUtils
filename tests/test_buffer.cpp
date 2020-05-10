//这个Buffer没写好
#include <Box/buffer.hpp>
int main(){
    Box::RingBuffer buf(5);
    char t[] = "Hello World";
    buf.write(t,sizeof(t));   
}