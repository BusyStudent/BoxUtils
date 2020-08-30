#include <curl/curl.h>
#include "common/def.hpp"
#include "curl/curl.hpp"
#include "libc/atexit.h"
namespace Box{
namespace Curl{
    void Init(bool init_all){
        long flags; 
        if(init_all){
            flags = CURL_GLOBAL_ALL;
        }
        else{
            flags = CURL_GLOBAL_SSL;
        }
        CURLcode code = curl_global_init(flags);
        if(flags != CURLE_OK){
            throwEasyException(code);
        }
        //初始化完成
        libc::atexit_once(Curl::Quit);
    }
    void Quit(){
        curl_global_cleanup();
    }
    const char *Version(){
        return curl_version();
    }
    std::string Get(std::string_view url){
        return Easy(url).content();
    }
};
};