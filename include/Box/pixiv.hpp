#ifndef _BOX_PIXIV_HPP_
#define _BOX_PIXIV_HPP_
#include <cstdint>
#include <string>
#include <optional>
#include "json.hpp"
namespace Box{
    namespace Curl{
        class EasyFactory;
        class Share;
    };
    namespace Pixiv{
        //实现
        using namespace Curl;
        struct ArtWorks;
        class Interface{
            //接口
            public:
                static constexpr int DefaultRetry = 5;
                Interface();//没有初始化
                Interface(EasyFactory *,Share *share = nullptr);//依赖于外部的工厂
                ~Interface();
                //得到内部的一下工厂
                Share &share() const noexcept{
                    return *share_s.share;
                };
                EasyFactory &factory() const noexcept{
                    return *factory_s.factory;
                };
                ArtWorks get_byid(uint64_t id);//得到通过ID
                //Http Get
                std::string http_get(std::string_view url);
            private:
                struct{
                    Share *share;
                    bool owned;//是否有所有权
                }share_s;//共享界面
                struct{
                    EasyFactory *factory;
                    bool owned;
                }factory_s;//工厂
                int max_retry;
        };
        //插画们
        struct Illusts{
            ~Illusts();
            int size() const;//有多少张
            std::optional<Json> info;
        };
        //插画的集合 页面
        struct ArtWorks{
            ~ArtWorks();
            //得到页面url
            std::string url() const;
            std::string title() const;

            Illusts pages();//得到Pages

            std::optional<Json> info;//内容
            Interface &interface;
            uint64_t id;
        };
        struct SearchResult{

        };
    };
};
#endif