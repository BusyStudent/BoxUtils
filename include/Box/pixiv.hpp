#ifndef _BOX_PIXIV_HPP_
#define _BOX_PIXIV_HPP_
#include <string>
#include <vector>
#include "json.hpp"
namespace Box{
    namespace Net{
        class EasyFactory;
        class Share;
    };
    namespace Pixiv{
        void Init();
        void Quit();
        bool IsInit();
        struct Anther{
            std::string name;
        };
        struct Page{
            static Page FromID(int id);//从ID
            static Page FromID(const std::string &id);
            static Page FromURL(const std::string &url);//从URL
            Json &operator [](const char *val) const;
            Json &operator [](int val) const;
            Json info;//格式化的信息
        };
        std::vector<Page> Search(const std::string &what);
        Net::EasyFactory &Factory();//得到内部facotory的引用
        Net::Share       &Share();//得到内部share的引用
    };
};
#endif