#ifndef _BOX_DUKTAPE_HPP_
#define _BOX_DUKTAPE_HPP_
struct duk_context;
namespace Box{
    namespace Duktape{
        class Context{
            public:
                Context();
                Context(const Context &) = delete;
                Context(Context &&);
                ~Context();
            private:
                duk_context *ctxt;
        };
    };
};

#endif