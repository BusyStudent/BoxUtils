#ifndef _BOX_DEF_HPP_
#define _BOX_DEF_HPP_
#include <cstddef>
namespace Box{
    namespace Size{
        //大小
        struct Capcity{
            //容量
            public:
                constexpr Capcity(size_t n):_value(n){}
                constexpr operator size_t(){
                    return _value;
                }
                constexpr size_t value() const noexcept{
                    return _value;
                }
            private:
                size_t _value;
        };
        struct Byte:public Capcity{
            using Capcity::Capcity;
        };
        //KB
        struct KB:public Capcity{
            constexpr KB(size_t n):Capcity(n * 1000){};
        };
        struct MB:public KB{
            constexpr MB(size_t n):KB(n * 1000){}
        };
        struct GB:public MB{
            constexpr GB(size_t n):MB(n * 1000){}
        };
        //KiB
        struct KiB:public Capcity{
            constexpr KiB(size_t n):Capcity(n * 1024){};
        };
        struct MiB:public KiB{
            constexpr MiB(size_t n):KiB(n * 1024){};
        };
        struct GiB:public MiB{
            constexpr GiB(size_t n):MiB(n * 1024){};
        };
    };
};
#endif