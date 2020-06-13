#include <Box/xml.hpp>
#include <Box/net.hpp>
#include <Box/xpath.hpp>
#include <Box/exception.hpp>
#include <vector>
#include <iostream>
int main(){
    using namespace Box;
    auto html = Html::ParseString(Net::Get("http://www.baidu.com"));
    //std::cout << html.to_string() << std::endl;
    //退出
    for(auto &node:html.xpath("//a")){
        std::cout << node["href"] << std::endl;
    }
    std::vector <Html::Node> nodeset;
    //收集到
    html.xpath("//a").collect_to(nodeset);
    Net::Quit();
    LXml::Quit();
}