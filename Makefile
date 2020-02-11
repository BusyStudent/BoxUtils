CXX=g++
CXXFLAGS=-g -fPIC
OBJ=random.o
LDFLAGS=
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
random.o:random.cpp random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c
clean:
	rm $(OBJ) libbox_utils.so