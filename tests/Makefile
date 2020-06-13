#测试的一些目标
CXX=g++
CXXFLAGS=-g
TARGET=test_buffer test_net_multi test_net_session test_assert test_json lxml test_event test_timer libc_test
TARGET+= thpool_test
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
test_event:test_event.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_event.cpp -o ./test_event -pthread
test_timer:test_timer.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) test_timer.cpp -o ./test_timer -pthread
libc_test:libc_test.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) libc_test.cpp -o ./libc_test
thpool_test:thpool_test.cpp
	$(CXX) $(CXXFLAGS) $(DEP_LIB) thpool_test.cpp -o ./thpool_test
clean:
	-rm $(TARGET)