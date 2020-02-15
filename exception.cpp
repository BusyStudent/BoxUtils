#include <sstream>
#include <stdexcept>
#include "exception.hpp"
BoxUtils::IndexError::IndexError(int index){
	this->index = index;
	std::stringstream stream;
	stream << "IndexError:out of range " << index;
	this->reason = stream.str();
}
const char*BoxUtils::IndexError::what()const throw(){
	return reason.c_str();
}

BoxUtils::KeyError::KeyError(const char *key){
	this->key = key;
}
const char *BoxUtils::KeyError::what()const throw(){
	return key.c_str();
}

BoxUtils::TypeError::TypeError(const char *excepted,const char *gived){
	//类型错误
	this->excepted = excepted;
	this->gived = gived;
	std::stringstream stream;
	stream << "excepted type:" <<"'"<< excepted <<"'"<<" gived type:" <<"'"<<gived<<"'";
	this->reason = stream.str();
}
const char *BoxUtils::TypeError::what() const throw(){
	return reason.c_str();
}
