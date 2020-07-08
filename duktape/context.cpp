#include <duktape.h>
#include "duktape/context.hpp"
namespace Box{
namespace Duktape{
    Context::Context(){
        //创建上下文
        ctxt = duk_create_heap_default();
    }
    Context::Context(Context &&context){
        ctxt = context.ctxt;
        context.ctxt = nullptr;
    }
    Context::~Context(){
        duk_destroy_heap(ctxt);
    }
};
};