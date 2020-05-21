#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "net_easy.hpp"
#include "net_factory.hpp"
#include "net_share.hpp"
#include "exception.hpp"
#include "string.hpp"
#include "pixiv.hpp"
#include <cctype>
/*
    日志
    
*/
namespace Box{
namespace Pixiv{
    //内部pixiv访问工具箱的实现
    using namespace Net;
    Net::EasyFactory *factory = nullptr;
    Net::Share       *share = nullptr;
    void Init(){
        //初始化
        if(factory == nullptr and share == nullptr){
            //检查一下
            factory = new Net::EasyFactory();
            share = new Net::Share();
            //共享连接和DNS
            share->set_share_connect();
            share->set_share_dns();
            share->set_share_ssl_session();
            share->set_share_psl();
        }
    };
    void Quit(){
        delete factory;
        delete share;
        factory = nullptr;
        share = nullptr;
    }
    bool IsInit(){
        //是否初始化
        return factory == nullptr and share == nullptr;
    }
    Net::EasyFactory &Factory(){
        return *factory;
    }
    Net::Share       &Share(){
        return *share;
    }
    //得到页面的信息
    Page Page::FromID(const std::string &id){
        auto req = factory->create(("https://www.pixiv.net/ajax/illust/"+id));
        //创建请求
        std::string content = req.content();
        //得到内容
        return Page{
            .info = Json::ParseString(content.c_str())
        };
    }
    Page Page::FromID(int id){
        return Page::FromID(std::to_string(id));
    }
    Page Page::FromURL(const std::string &url){
        //从URL得到
        auto vec = String::Split(url.c_str(),"/");
        for(auto &str:vec){
            //遍历一下
            for(auto &ch:str){
                //整个字符串是否是数字
                if(isdigit(ch) != 0){
                    continue;
                }
                return Page::FromID(str);
            }
        }
        throw InvalidArgument("非法URL");
    }
};
};