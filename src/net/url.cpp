//解析URL
#include <string>
#include "common/def.hpp"
#include "net/url.hpp"
namespace Box{
namespace Net{
    Url::Url(const std::string &url){
        //解析字符串
        std::string::const_iterator iter = url.begin();
        while(iter != url.end() and *iter == ' '){
            ++ iter;
        }
        //去掉空格
        
    }
    Url::~Url(){}
};
} // namespace Box
