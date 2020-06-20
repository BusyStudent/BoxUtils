#ifndef _BOX_LIBEVENT_HPP_
#define _BOX_LIBEVENT_HPP_
struct event_base;
namespace Box{
    namespace Libevent{
        enum class BaseStatus{
            TIMEOUT = 1,
            OK      = 0,
            ERR     = -1
        };
        class Base{
            public:
                Base();
                Base(const Base&) = delete;
                Base(Base &&);
                ~Base();
                const char *get_method() const noexcept;//得到方法
                BaseStatus loop();//循环
                BaseStatus dispatch();//分发事件
            private:
                event_base *base;
        };

    };
};
#endif