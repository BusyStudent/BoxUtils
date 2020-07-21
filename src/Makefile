AR=ar
CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++ -std=c++17
CXXFLAGS=-g -fPIC -Wall -Wextra
INCLUDE=../include/Box/
#主要的对象
OBJ=random.o cJSON.o cJSON_Utils.o json.o net.o net/easy.o net/factory.o net/headers.o net/exception.o\
	exception.o iconv.o sync/sem.o base64.o socket.o string.o dns.o filesystem.o net/multi.o\
	xml.o xpath.o sax.o timer.o os.o backtrace.o net/share.o\
	dylib.o logger.o subprocess.o buffer.o\
	assert.o threadpool.o fmt.o net/url.o\
	co/context.o co/ucontext.o
LIBC_OBJ=libc/mem.o libc/inet.o libc/atexit.o libc/io.o

OBJ+=$(LIBC_OBJ)

#SDL的对象
#SDL_OBJ=sdl/sdl.o
#SDL_LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

LDFLAGS=-pthread -lcurl `xml2-config --libs`
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
CXXFLAGS+=-I $(INCLUDE)
CFLAGS+=-I $(INCLUDE)
all:libbox_utils.so
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
libbox_utils.a:$(OBJ)
	$(AR) rv libbox_utils.a $(OBJ)
#libbox_sdl.so:$(SDL_OBJ)
#	$(CXX) $(SDL_LDFLAGS) $(SDL_OBJ) -shared -o libbox_sdl.so
random.o:random.cpp $(INCLUDE)random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c
os.o:os.cpp $(INCLUDE)os.hpp
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
net/factory.o:net/factory.cpp $(INCLUDE)net/factory.hpp
net/exception.o:net/exception.cpp $(INCLUDE)net/exception.hpp
net/easy.o:net/easy.cpp $(INCLUDE)net/easy.hpp
net/multi.o:net/multi.cpp $(INCLUDE)net/multi.hpp
net/share.o:net/share.cpp $(INCLUDE)net/share.hpp
net/url.o:net/url.cpp $(INCLUDE)net/url.hpp
#异常
excepton.o:exception.cpp $(INCLUDE)exception.hpp
iconv.o:iconv.cpp $(INCLUDE)iconv.hpp
#字符串编码
#线程同步通讯
sync/sem.o:sync/sem.cpp $(INCLUDE)sync/sem.hpp

base64.o:base64.cpp $(INCLUDE)base64.hpp
socket.o:socket.cpp $(INCLUDE)socket.hpp
string.o:string.cpp $(INCLUDE)string.hpp
dns.o:dns.cpp $(INCLUDE)dns.hpp
filesystem.o:filesystem.cpp $(INCLUDE)filesystem.hpp
#计时器
timer.o:timer.cpp sync/sem.o $(INCLUDE)timer.hpp
xml.o:xml.cpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xml.cpp -c
xpath.o:xpath.cpp $(INCLUDE)xpath.hpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) xpath.cpp -c
sax.o:sax.cpp $(INCLUDE)sax.hpp $(INCLUDE)xml.hpp
	$(CXX) $(CXXFLAGS) $(XMLFLAGS) sax.cpp -c
#XML解析
backtrace.o:backtrace.cpp $(INCLUDE)backtrace.hpp

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
#断言的实现
assert.o:assert.cpp $(INCLUDE)assert.hpp
#流抽象
#stream.o:stream.cpp $(INCLUDE)stream.hpp
#线程池子
threadpool.o:threadpool.cpp $(INCLUDE)threadpool.hpp
#格式化
fmt.o:fmt.cpp $(INCLUDE)fmt.hpp
#一些C函数文件
libc/mem.o:libc/mem.c $(INCLUDE)libc/mem.h
	$(CC) $(CFLAGS) libc/mem.c -c -o ./libc/mem.o
libc/inet.o:libc/inet.c $(INCLUDE)libc/inet.h
	$(CC) $(CFLAGS) libc/inet.c -c -o ./libc/inet.o
libc/atexit.o:libc/atexit.c $(INCLUDE)libc/atexit.h
	$(CC) $(CFLAGS) libc/atexit.c -c -o ./libc/atexit.o
libc/io.o:libc/io.c $(INCLUDE)libc/io.h
	$(CC) $(CFLAGS) libc/io.c -c -o ./libc/io.o
#协程的文件
co/context.o:co/context.cpp $(INCLUDE)co/context.hpp
co/ucontext.o:co/ucontext.cpp $(INCLUDE)co/ucontext.h
clean:
	-rm $(OBJ) libbox_utils.so
