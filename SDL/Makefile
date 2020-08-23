CXX=g++
CXXFLAGS=-fPIC -g `sdl2-config --cflags`
LDFLAGS=`sdl2-config --libs` -lSDL2_image
INCLUDE=../include/Box/

OBJ=window.o exception.o render.o

CXXFLAGS+=-I $(INCLUDE)

libbox_sdl.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_sdl.so

window.o:window.cpp
	$(CXX) $(CXXFLAGS) window.cpp -c
render.o:render.cpp
exception.o:exception.cpp

clean:
	-rm $(OBJ)
	-rm libbox_sdl.so