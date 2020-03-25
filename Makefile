CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++
CXXFLAGS=-g -fPIC -Wall -Wextra
OBJ=random.o cJSON.o json.o net.o net_easy.o net_factory.o net_headers.o net_exception.o\
	exception.o iconv.o sem.o base64.o socket.o string.o dns.o xv.o filesystem.o net_multi.o\
	xml.o xpath.o time.o\
	sdl_rect.o sdl_pixels.o sdl_surface.o sdl_rwops.o sdl_video.o sdl_msgbox.o
LDFLAGS=-pthread -lcurl `xml2-config --libs` `sdl2-config --libs`
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
SDLFLAGS=`sdl2-config --cflags`
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
net_multi.o:net_multi.cpp net_multi.hpp
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
#XML解析
#SDL的封装
#SDL的矩形操作
sdl_rect.o:sdl_rect.cpp sdl_rect.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_rect.cpp -c
#像素操作
sdl_pixels.o:sdl_pixels.cpp sdl_pixels.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_pixels.cpp -c
#Surface操作
sdl_surface.o:sdl_surface.cpp sdl_surface.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_surface.cpp -c
#文件抽象
sdl_rwops.o:sdl_rwops.cpp sdl_rwops.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_rwops.cpp -c
#视频(窗口操作)
sdl_video.o:sdl_video.cpp sdl_video.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_video.cpp -c
#SDL的信息框(不大好看)
sdl_msgbox.o:sdl_msgbox.cpp sdl_msgbox.hpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) sdl_msgbox.cpp -c
clean:
	rm $(OBJ) libbox_utils.so
