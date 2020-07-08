#ifndef _BOX_CRYPTO_SHA1_HPP_
#define _BOX_CRYPTO_SHA1_HPP_
//Sha1算法实现
#include <cstdint>
namespace Box{
    namespace Crypto{
        class Sha1Context{
            uint8_t msg[64];//信息
        };
    };
};
#endif