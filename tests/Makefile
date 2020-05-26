#测试的一些目标
CXX=g++
CXXFLAGS=-g
TARGET=test_buffer test_net_multi test_net_session test_assert test_json lxml
DEP_LIB=../src/libbox_utils.so
all:$(TARGET)
test_buffer:test_buffer.cpp $(DEP_LIB)
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_buffer.cpp -o ./test_buffer
test_net_multi:test_net_multi.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_net_multi.cpp -o ./test_net_multi
test_net_session:test_net_session.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_net_session.cpp -o ./test_net_session
test_assert:test_assert.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_assert.cpp -o ./test_assert
test_json:test_json.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_json.cpp -o ./test_json
lxml:lxml.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) lxml.cpp -o ./lxml
clean:
	-rm $(TARGET)