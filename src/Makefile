AR=ar
CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
CXX=g++ -std=c++17
CXXFLAGS=-g -fPIC -Wall -Wextra
INCLUDE=../include/Box/
#主要的对象
OBJ=random.o cJSON.o cJSON_Utils.o json.o net.o net/easy.o net/factory.o net/headers.o net/exception.o\
	exception.o iconv.o sem.o base64.o socket.o string.o dns.o filesystem.o net/multi.o\
	xml.o xpath.o sax.o timer.o os.o backtrace.o net/share.o\
	coroutine.o scheduler.o dylib.o logger.o subprocess.o buffer.o ucontext.o\
	assert.o pixiv.o threadpool.o
LIBC_OBJ=libc/mem.o libc/inet.o libc/atexit.o libc/io.o
LIBEVENT_OBJ=libevent/base.o

OBJ+=$(LIBC_OBJ)
OBJ+=$(LIBEVENT_OBJ)

#SDL的对象
#SDL_OBJ=sdl/sdl.o
#SDL_LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

#QuickJS
QJS_OBJ=quickjs/runtime.o quickjs/context.o quickjs/value.o
QJS_LDFLAGS=-L./ -lbox_utils -lquickjs -lm


LDFLAGS=-pthread -lcurl `xml2-config --libs` -levent
XMLFLAGS=`xml2-config --cflags`
#LIBXML的配置文件
CXXFLAGS+=-I $(INCLUDE)
CFLAGS+=-I $(INCLUDE)
all:libbox_utils.so libbox_quickjs.so
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
libbox_utils.a:$(OBJ)
	$(AR) rv libbox_utils.a $(OBJ)
#libbox_sdl.so:$(SDL_OBJ)
#	$(CXX) $(SDL_LDFLAGS) $(SDL_OBJ) -shared -o libbox_sdl.so
libbox_quickjs.so:$(QJS_OBJ)
	$(CXX) $(QJS_LDFLAGS) $(QJS_OBJ) -shared -o libbox_quickjs.so
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
#异常
excepton.o:exception.cpp $(INCLUDE)exception.hpp
iconv.o:iconv.cpp $(INCLUDE)iconv.hpp
#字符串编码
sem.o:sem.cpp $(INCLUDE)sem.hpp
#信号量
base64.o:base64.cpp $(INCLUDE)base64.hpp
socket.o:socket.cpp $(INCLUDE)socket.hpp
string.o:string.cpp $(INCLUDE)string.hpp
dns.o:dns.cpp $(INCLUDE)dns.hpp
filesystem.o:filesystem.cpp $(INCLUDE)filesystem.hpp
#计时器
timer.o:timer.cpp sem.o $(INCLUDE)timer.hpp
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
pixiv.o:pixiv.cpp $(INCLUDE)pixiv.hpp net/easy.o net/factory.o json.o
#流抽象
#stream.o:stream.cpp $(INCLUDE)stream.hpp
#线程池子
threadpool.o:threadpool.cpp $(INCLUDE)threadpool.hpp
#一些C函数文件
libc/mem.o:libc/mem.c $(INCLUDE)libc/mem.h
	$(CC) $(CFLAGS) libc/mem.c -c -o ./libc/mem.o
libc/inet.o:libc/inet.c $(INCLUDE)libc/inet.h
	$(CC) $(CFLAGS) libc/inet.c -c -o ./libc/inet.o
libc/atexit.o:libc/atexit.c $(INCLUDE)libc/atexit.h
	$(CC) $(CFLAGS) libc/atexit.c -c -o ./libc/atexit.o
libc/io.o:libc/io.c $(INCLUDE)libc/io.h
	$(CC) $(CFLAGS) libc/io.c -c -o ./libc/io.o
clean:
	-rm $(OBJ) libbox_utils.so
	-rm $(QJS_OBJ)
