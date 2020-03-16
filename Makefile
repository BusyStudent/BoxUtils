CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++
CXXFLAGS=-g -fPIC -Wall -Wextra
OBJ=random.o cJSON.o json.o net.o net_easy.o net_factory.o net_headers.o net_exception.o\
	exception.o iconv.o sem.o base64.o socket.o string.o dns.o xv.o filesystem.o\
	xml.o xpath.o
LDFLAGS=-pthread -lcurl `xml2-config --libs`
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
random.o:random.cpp random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c

json.o:json.cpp json.hpp cJSON.o exception.o
#Json解析器和构建器
cJSON.o:cJSON.c cJSON.h
	$(CC) $(CFLAGS) cJSON.c -c
#依赖的cJSON
net.o:net.cpp net.hpp
net_headers.o:net_headers.cpp net_headers.hpp
#net_callback.o:net_callback.cpp net.hpp
#net_response.o:net_response.cpp net.hpp
net_factory.o:net_factory.cpp net_factory.hpp
net_exception.o:net_exception.cpp net_exception.hpp
net_easy.o:net_easy.cpp net_easy.hpp
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
xml.o:xml.cpp xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xml.cpp -c
xpath.o:xpath.cpp xpath.hpp xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xpath.cpp -c
#XML解析
clean:
	rm $(OBJ) libbox_utils.so
