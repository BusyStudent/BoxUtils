#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "net/easy.hpp"
#include "net/factory.hpp"
#include "net/exception.hpp"
#include "net/share.hpp"
#include "exception.hpp"
#include "string.hpp"
#include "pixiv.hpp"
#include "fmt.hpp"
#include <cctype>
/*
    日志
    
*/
namespace Box{
namespace Pixiv{
    Interface::~Interface(){
        //删除内部的网络接口
        if(share_s.owned){
            delete share_s.share;
        }
        if(factory_s.owned){
            delete factory_s.factory;
        }
    }
    Interface::Interface():Interface(nullptr,nullptr){
        share().set_share_all();
        //设置referer
        factory().add_header("Referer","https://www.pixiv.net/");
    }
    Interface::Interface(EasyFactory *f,Share *s){
        //初始化
        if(f == nullptr){
            factory_s.factory = new EasyFactory();
            factory_s.owned = true;
        }
        else{
            factory_s.factory = f;
            factory_s.owned = false;
        }

        if(s == nullptr){
            share_s.share = new Share();
            share_s.owned = true;
        }
        else{
            share_s.share = s;
            share_s.owned = false;
        }
    }
    Item Interface::get_byid(uint64_t id){
        std::string target = Format("https://www.pixiv.net/ajax/illust/{}",id);
        //得到目标网址
        int i = 0;
        while(true){
            try{
                auto pak = factory().create(target);
                Json json = Json::ParseString(pak.content());
                if(json["error"] == true){
                    //出错了
                    Printfln("Get {} failed\n",target);
                    Printfln("json -> {}",json);
                }
                else{
                    return Item{
                        .info = json,
                        .id = id
                    };
                }
            }
            catch(Net::EasyException &err){
                i ++;
                if(i == max_retry){
                    //失败
                    throw;
                }
                continue;
            }
        }
    }
    std::string Item::url() const{
        return Format("https://www.pixiv.net/ajax/illust/{}",id);
    }
    std::string Item::title() const{
        //得到标题
        return info.value()["body"]["illustTitle"];
    }
    Item::~Item(){

    }
    Illust Item::operator [](int index) const{
        return {
            info.value()["body"][index]
        };
    }
};
};
