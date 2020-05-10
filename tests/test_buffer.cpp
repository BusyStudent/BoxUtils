#include "buffer.hpp"
#include "buffer.cpp"
int main(){
    Box::RingBuffer buf(5);
    char t[] = "Hello World";
    buf.write(t,sizeof(t));   
}