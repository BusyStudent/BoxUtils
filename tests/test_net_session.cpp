#include <Box/net/session.hpp>
#include <Box/net.hpp>
int main(){
    std::string s;
    Box::Net::Session session;
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.get("https://www.baidu.com").done();
    session.wait_all();
    Box::Net::Quit();
}