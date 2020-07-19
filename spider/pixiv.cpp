#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "net/easy.hpp"
#include "net/factory.hpp"
#include "net/share.hpp"
#include "exception.hpp"
#include "string.hpp"
#include "pixiv.hpp"
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
        share_s.share->set_share_all();
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
};
};
