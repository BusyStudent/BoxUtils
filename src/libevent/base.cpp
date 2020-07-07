#include <event2/event.h>
#include "libevent/base.hpp"
namespace Box{
    namespace Libevent{
        void Quit(){
            libevent_global_shutdown();
        }
        Base::Base(){
            base = event_base_new();
        }
        Base::Base(Base &&b){
            //移动
            base = b.base;
            b.base = nullptr;
        }
        Base::~Base(){
            if(base != nullptr){
                event_base_free(base);
            }
        }
        const char *Base::get_method() const noexcept{
            return event_base_get_method(base);
        }
        //分发事件
        BaseStatus Base::dispatch(){
            return static_cast<BaseStatus>(event_base_dispatch(base));
        }
    };
};