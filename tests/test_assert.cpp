#include <Box/assert.hpp>
#include <iostream>
#include <string>
int main(){
    std::string str;
    std::cin >> str;
    BOX_ASSERT(str == "Hello");
}