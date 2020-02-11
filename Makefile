CXX=g++
CXXFLAGS=-g -fPIC
OBJ=random.o net.o net_session.o net_headers.o
LDFLAGS=-lcurl
libbox_utils.so:$(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -shared -o libbox_utils.so
random.o:random.cpp random.hpp
	$(CXX) $(CXXFLAGS) random.cpp -c
net.o:net.cpp net.hpp
net_session.o:net_session.cpp net.hpp
net_headers.o:net_headers.cpp net.hpp
clean:
	rm $(OBJ) libbox_utils.so
