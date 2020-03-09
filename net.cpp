#include "net_session.hpp"
#include "net.hpp"
using namespace Box;
Net::Session *Net::global_session = nullptr;
void Net::Init(){
	Net::global_session = new Net::Session();
}
void Net::Quit(){
	delete Net::global_session;
}
