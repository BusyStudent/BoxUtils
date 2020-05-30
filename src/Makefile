CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++ -std=c++17
CXXFLAGS=-g -fPIC -Wall -Wextra
INCLUDE=../include/Box/
OBJ=random.o cJSON.o cJSON_Utils.o json.o net.o net_easy.o net_factory.o net_headers.o net_exception.o\
	exception.o iconv.o sem.o base64.o socket.o string.o dns.o xv.o filesystem.o net_multi.o\
	xml.o xpath.o sax.o time.o os.o backtrace.o net_share.o net_session.o\
	coroutine.o scheduler.o dylib.o logger.o subprocess.o buffer.o ucontext.o stream.o\
	assert.o pixiv.o
LDFLAGS=-pthread -lcurl `xml2-config --libs`
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
CXXFLAGS+=-I $(INCLUDE)
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
random.o:random.cpp $(INCLUDE)random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c
os.o:os.cpp $(INCLUDE)os.hpp linux.cpp $(INCLUDE)linux.hpp
#操作系统的操作 目前只有Linux
json.o:json.cpp $(INCLUDE)json.hpp cJSON.o exception.o
#Json解析器和构建器
cJSON.o:cJSON.c cJSON.h
	$(CC) $(CFLAGS) cJSON.c -c
cJSON_Utils.o:cJSON_Utils.c cJSON_Utils.h
	$(CC) $(CFLAGS) cJSON_Utils.c -c
#依赖的cJSON
net.o:net.cpp $(INCLUDE)net.hpp
net_headers.o:net_headers.cpp $(INCLUDE)net/headers.hpp
#net_callback.o:net_callback.cpp net.hpp
#net_response.o:net_response.cpp net.hpp
net_factory.o:net_factory.cpp $(INCLUDE)net/factory.hpp
net_exception.o:net_exception.cpp $(INCLUDE)net/exception.hpp
net_easy.o:net_easy.cpp $(INCLUDE)net/easy.hpp
net_multi.o:net_multi.cpp $(INCLUDE)net/multi.hpp
net_share.o:net_share.cpp $(INCLUDE)net/share.hpp
net_session.o:net_session.cpp $(INCLUDE)net/session.hpp
#异常
excepton.o:exception.cpp $(INCLUDE)exception.hpp
iconv.o:iconv.cpp $(INCLUDE)iconv.hpp
#字符串编码
sem.o:sem.cpp $(INCLUDE)sem.hpp
#信号量
base64.o:base64.cpp $(INCLUDE)base64.hpp
socket.o:socket.cpp $(INCLUDE)socket.hpp
xv.o:xv.cpp $(INCLUDE)xv.hpp socket.o
#简单封装服务器和客户端
string.o:string.cpp $(INCLUDE)string.hpp
dns.o:dns.cpp $(INCLUDE)dns.hpp
filesystem.o:filesystem.cpp $(INCLUDE)filesystem.hpp
time.o:time.cpp $(INCLUDE)time.hpp
xml.o:xml.cpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xml.cpp -c
xpath.o:xpath.cpp $(INCLUDE)xpath.hpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xpath.cpp -c
sax.o:sax.cpp $(INCLUDE)sax.hpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) sax.cpp -c
#XML解析
backtrace.o:backtrace.cpp $(INCLUDE)backtrace.hpp
coroutine.o:coroutine.cpp $(INCLUDE)coroutine.hpp
#协程的实现 基于ucontext
scheduler.o:scheduler.cpp $(INCLUDE)scheduler.hpp
#调度器
dylib.o:dylib.cpp $(INCLUDE)dylib.hpp
#动态库
#同步用的通道
#channal.o:channal.cpp $(INCLUDE)channal.hpp
#日志记录器
logger.o:logger.cpp $(INCLUDE)logger.hpp
subprocess.o:subprocess.cpp $(INCLUDE)subprocess.hpp
#缓冲区
buffer.o:buffer.cpp $(INCLUDE)buffer.hpp
#ucontext封装
ucontext.o:ucontext.cpp $(INCLUDE)ucontext.hpp
#断言的实现
assert.o:assert.cpp $(INCLUDE)assert.hpp
#对Pixiv访问的接口
pixiv.o:pixiv.cpp $(INCLUDE)pixiv.hpp net_easy.o net_factory.o json.o
#流抽象
stream.o:stream.cpp $(INCLUDE)stream.hpp
clean:
	rm $(OBJ) libbox_utils.so
