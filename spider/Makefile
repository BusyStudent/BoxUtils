CXX=g++
CXXFLAGS=-g -std=c++17 -fPIC
INCLUDE=../include/Box/

OBJ=pixiv.o
LDFLAGS=-lbox_utils

#爬虫的API
libbox_spider.so:$(OBJ) ../src/libbox_utils.so
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_spider.so

pixiv.o:pixiv.cpp $(INCLUDE)pixiv.hpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDE) -c pixiv.cpp

../src/libbox_utils.so:
	cd ../src/ && make