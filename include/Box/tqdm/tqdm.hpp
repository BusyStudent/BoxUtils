#if !defined(_BOX_TQDM_HPP_)
#define _BOX_TQDM_HPP_
#include <cstdio>
#include <utility>
#include "../link.h"
namespace Box{
    namespace TqdmImpl{
        template<class T>
        struct Iterator{
            
        };
        struct Tqdm{
            FILE *console;
            double value;//进度数值 到100就满了
            //更新数值
            void update(double new_val);
        };
    };
    template<class T>
    class Tqdm{
        public:
            Tqdm(T &&obj):obj(std::forward<T>(obj)){};
            //迭代器
            auto begin() const noexcept{
                return obj.begin();
            };
            auto end() const noexcept{
                return obj.end();
            };
        private:
            T &obj;//对象
    };
} // namespace Box



#endif // _BOX_TQDM_HPP_
