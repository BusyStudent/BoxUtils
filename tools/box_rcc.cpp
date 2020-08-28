#include <Box/libc/mem.h>
#include <Box/json.hpp>
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
void process(Json &json,FILE *);
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
    //解析
    Json json = Json::ParseString(content);
    //加载内容
    process(json,stdout);
}
std::string load_file(std::string_view filename){
    std::ifstream s(filename.data());
    std::istreambuf_iterator<char> begin(s),end;
    std::string content(begin,end);
    return content;
}
void process(Json &json,FILE *output){
    enum class Lang{
        C,
        CXX
    };
    Lang lang;
    if(not json.has("lang")){
        Printfln(stderr,"Warning:Input does not set lang,default = c++");
        lang = Lang::CXX;
    }
    else{
        std::string_view language = json["lang"];
        if(libc::strcasecmp(language.data(),"c") == 0){
            lang = Lang::C;
        }
        else if(libc::strcasecmp(language.data(),"c++") == 0){
            lang = Lang::CXX;
        }
        else if(libc::strcasecmp(language.data(),"cpp") == 0){
            lang = Lang::CXX;
        }
        else if(libc::strcasecmp(language.data(),"default") == 0){
            lang = Lang::CXX;
        }
        else{
            Printfln(stderr,"Error:Unknown language {}",language);
            exit(EXIT_FAILURE);
        }
    }
    Printfln(output,"/*Auto generate by box-rcc*/");
    Printfln(output,"#include <stdint.h>");
    switch(lang){
        case Lang::C:{
            //C的保护部分
            Printfln(output,R"(
                #ifdef __cplusplus
                extern "C"{
                #endif
            )");
        }
    }
    for(auto &item:json){
        if(item.name() != "lang"){
            //解析一下
            std::string_view type = item["type"];
            if(type == "string"){
                //字符串
                std::string txt = load_file(item["file"]);
                Printfln("char {}[] = R\"({})\";",item.name(),txt);
            }
            else if(type == "binary"){
                //2进制
                FILE *f = fopen(item["file"],"rb");
                if(f == nullptr){
                    Printfln(stderr,"Error:Failed open {}",item["file"]);
                    exit(EXIT_FAILURE);
                }
                Printf(output,"uint8_t {}[] = {",item.name());
                int ch = fgetc(f);
                while(ch != EOF){
                    fprintf(f,",&x",ch);
                    ch = fgetc(f);
                }
                Printfln(output,"}");
                fclose(f);
            }
            else{
                Printfln(stderr,"Error:Unknown type {}",type);
                exit(EXIT_FAILURE);
            }
        }
    }
    switch(lang){
        case Lang::C:{
            //C的保护部分
            Printfln(output,R"(
                #ifdef __cplusplus
                }
                #endif
            )");
        }
    }
}