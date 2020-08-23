#if !defined(_BOX_RAII_SCOPE_HPP_)
#define _BOX_RAII_SCOPE_HPP_
//ScopeGuard
//deferer
#define BOX_DEFER Box::RAII::Defer __FL_DEFERER__  = [&]()
#define Box_defer BOX_DEFER
namespace Box{
    namespace RAII{
        //ScopeGuard的实现
        template<class T>
        class ScopeGuard{
            public:
                ScopeGuard(T &&func):fn(func),released(false){
                    //创建函数
                };
                ScopeGuard(const ScopeGuard &) = delete;
                ~ScopeGuard(){
                    if(not released){
                        fn();
                    }
                };
                //不需要执行
                void release() noexcept{
                    released = true;
                };
            private:
                T fn;//函数
                bool released;
        };
        //这个不能release
        template<class T>
        class Defer{
            public:
                Defer(T &&func):fn(func){};
                Defer(const Defer &) = delete;
                ~Defer(){
                    fn();
                };
            private:
                T fn;
        };
    };
    using RAII::ScopeGuard;
};


#endif // _BOX_RAII_SCOPE_HPP_
