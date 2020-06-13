#include <Box/json.hpp>
#include <iostream>
int main(){
    auto json = Box::Json::LoadFile("test.json");
    for(auto &val:json){
        std::cout << val.get_name() <<':'<< val.to_string() << std::endl;
    }
    std::cout << json["Hello"].to_string() << std::endl;
    json["Hello"] = "你好";
    std::cout << json["Hello"].to_string() << std::endl;
    Box::Json js = {
        0,1,5,4
    };
    for(auto &val:json["value"]){
        std::cout << int(val) << std::endl;
    }
    std::cout << js.to_string() << std::endl;
}