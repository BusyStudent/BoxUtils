#ifndef _BOX_PIXIV_HPP_
#define _BOX_PIXIV_HPP_
#include <cstdint>
#include <string>
#include <optional>
#include "json.hpp"
namespace Box{
    namespace Net{
        class EasyFactory;
        class Share;
    };
    namespace Pixiv{
        //实现
        using namespace Net;
        struct Item;
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
                Item get_byid(uint64_t id);//得到通过ID
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
        //插画
        struct Illust{
            JsonRef ref;
        };
        //插画的集合 页面
        struct Item{
            ~Item();
            //得到页面url
            std::string url() const;
            std::string title() const;

            Illust operator [](int index) const;

            std::optional<Json> info;//内容
            uint64_t id;
        };
        struct SearchResult{

        };
    };
};
#endif