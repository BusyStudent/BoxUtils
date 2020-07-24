#include <Box/sax.hpp>
#include <Box/xml.hpp>
#include <Box/fmt.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace Box;
[[noreturn ]]void show_help(){
    std::cerr << "BoxUtils Resource compiler version 0.1" << std::endl;
    std::cerr << "useage: box_rcc [options]... [file]..." << std::endl;
    std::cerr << "  {filename}    complie file" << std::endl;
    std::cerr << "  -             read from stdin" << std::endl;
    std::cerr << "  -o {filename} output file"  << std::endl;
    exit(EXIT_FAILURE);
}
//比较参数
inline bool cmp_argv(const char *arg,const char *s2){
    return strncasecmp(arg,s2,strlen(s2)) == 0;
}
int main(int argc,char **argv){
    //内容
    std::string content;
    std::string input_file;
    std::string output_file;
    if(argc == 1){
        show_help();
    }
    else{
        for(int i = 1;i < argc; i ++){
            if(cmp_argv(argv[i],"-o")){
                if(i + 1 >= argc){
                    //参数不够
                    show_help();
                }
                output_file = argv[i + 1];
                i ++;
            }
            else if(strcmp(argv[i],"--help") == 0){
                show_help();
            }
            else if(strcmp(argv[i],"-") == 0){
                //read from stdin
                input_file = "-";
            }
            else{
                input_file = argv[i];
            }
        }
    }
    std::stringstream stream;
    //读入
    if(input_file == "-"){
        std::istreambuf_iterator<char> begin(std::cin),end;
        content.assign(begin,end);
        if(std::cin.bad()){
            std::cerr << "Failed to read stdin" << std::endl;
            return EXIT_FAILURE;
        }
    }
    else{
        std::fstream f(input_file);
        std::istreambuf_iterator<char> begin(f),end;
        content.assign(begin,end);
        if(f.bad()){
            std::cerr << Format("Failed to read {}",input_file) << std::endl;
            return EXIT_FAILURE;
        }
    }
    //处理
    struct{
        std::string lang;//语言
    }res;
    //解析

    LXml::Xml xml = LXml::Xml::ParseString(content);
    //得到主要的
    LXml::Node res = xml["res"];
}