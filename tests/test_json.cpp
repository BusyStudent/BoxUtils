#include <Box/json.hpp>
#include <Box/fmt.hpp>
#include <iostream>
int main(){
    Box::Json json = Box::Json::LoadFile("test.json");
    for(auto &val:json){
        std::cout << val.name() <<':'<< val.to_string() << std::endl;
    }
    std::cout << json["Hello"].to_string() << std::endl;
    json["Hello"] = "你好";
    std::cout << json["Hello"].to_string() << std::endl;
    
    for(auto &val:json["value"]){
        std::cout << int(val) << std::endl;
    }
    json.insert("Hello CXX",545454);
    
    json.insert("MyTable",{
        10,"A",{
            {"Hello"}
        }
    });
    std::cout << json.to_string() << std::endl;
    json["MyTable"] = {
        "A",2,{{{{{"AAAAAAAAAAAAA"}}}}}
    };
    std::cout << json.get_path(json["value"][2]) << std::endl;
    std::cout << json.to_string() << std::endl;
    //auto iter = js.begin();
    //iter = js.erase(iter);
    //std::cout << iter->to_string() << std::endl;
    //
    Box::Fmt::Printfln("Json -> {}",json);
    Box::Fmt::Printfln("{}",true);
}