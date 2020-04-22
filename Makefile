CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++
CXXFLAGS=-g -fPIC -Wall -Wextra
OBJ=random.o cJSON.o cJSON_Utils.o json.o net.o net_easy.o net_factory.o net_headers.o net_exception.o\
	exception.o iconv.o sem.o base64.o socket.o string.o dns.o xv.o filesystem.o net_multi.o\
	xml.o xpath.o sax.o time.o os.o backtrace.o net_share.o net_session.o\
	coroutine.o scheduler.o dylib.o channal.o logger.o
LDFLAGS=-pthread -lcurl `xml2-config --libs`
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
random.o:random.cpp random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c
os.o:os.cpp os.hpp linux.cpp linux.hpp
#操作系统的操作 目前只有Linux
json.o:json.cpp json.hpp cJSON.o exception.o
#Json解析器和构建器
cJSON.o:cJSON.c cJSON.h
	$(CC) $(CFLAGS) cJSON.c -c
cJSON_Utils.o:cJSON_Utils.c cJSON_Utils.h
	$(CC) $(CFLAGS) cJSON_Utils.c -c
#依赖的cJSON
net.o:net.cpp net.hpp
net_headers.o:net_headers.cpp net_headers.hpp
#net_callback.o:net_callback.cpp net.hpp
#net_response.o:net_response.cpp net.hpp
net_factory.o:net_factory.cpp net_factory.hpp
net_exception.o:net_exception.cpp net_exception.hpp
net_easy.o:net_easy.cpp net_easy.hpp
net_multi.o:net_multi.cpp net_multi.hpp
net_share.o:net_share.cpp net_share.hpp
net_session.o:net_session.cpp net_session.hpp
#异常
excepton.o:exception.cpp exception.hpp
iconv.o:iconv.cpp iconv.hpp
#字符串编码
sem.o:sem.cpp sem.hpp
#信号量
base64.o:base64.cpp base64.hpp
socket.o:socket.cpp socket.hpp
xv.o:xv.cpp xv.hpp socket.o
#简单封装服务器和客户端
string.o:string.cpp string.hpp
dns.o:dns.cpp dns.hpp
filesystem.o:filesystem.cpp filesystem.hpp
time.o:time.cpp time.hpp
xml.o:xml.cpp xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xml.cpp -c
xpath.o:xpath.cpp xpath.hpp xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xpath.cpp -c
sax.o:sax.cpp sax.hpp xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) sax.cpp -c
#XML解析
backtrace.o:backtrace.cpp backtrace.hpp
coroutine.o:coroutine.cpp coroutine.hpp
#协程的实现 基于ucontext
scheduler.o:scheduler.cpp scheduler.hpp
#调度器
dylib.o:dylib.cpp dylib.hpp
#动态库
#同步用的通道
channal.o:channal.cpp channal.hpp
#日志记录器
logger.o:logger.cpp logger.hpp
clean:
	rm $(OBJ) libbox_utils.so
