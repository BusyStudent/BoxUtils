//NET Multi测试
#include <Box/net/multi.hpp>
#include <Box/net/share.hpp>
#include <Box/net/easy.hpp>
#include <Box/net.hpp>
#include <cstdio>
using namespace Box::Net;
int main(){
    Share share;
    share.set_share_all();
    Multi m;
    //创建多接口
    //复制一下
    Easy easy;
    share.add_handle(easy);
    easy.set_url("https://www.baidu.com");
    m.add_handle(easy,new int);
    share.add_handle(easy);
    Easy e2 = easy;
    m.add_handle(e2,new int);
    share.add_handle(e2);
    Easy e3 = easy;
    m.add_handle(e3,new int);
    share.add_handle(e3);
    int handle;
    m.perform(handle);
    while(handle){
        //handle不为0时候调用
        m.wait(100);
        m.perform(handle);
        m.for_msg([](MultiMsg &msg){
            msg.multi->remove_handle(msg.easy);
            printf("%p\n",msg.userdata);
            delete (int*)msg.userdata;
        });
    }
    Quit();
}