//解析URL
#include <string>
namespace Box{
    namespace Net{
        class Url{
            public:
                Url(const std::string &url);
                ~Url();
                //得到数据
                std::string href() const noexcept{
                    return _href;
                };
            private:
                std::string _href;
        };
    };
} // namespace Box
