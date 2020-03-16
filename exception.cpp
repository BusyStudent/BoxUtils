#include <sstream>
#include <stdexcept>
#include "exception.hpp"
Box::IndexError::IndexError(int index){
	this->index = index;
	std::stringstream stream;
	stream << "IndexError:out of range " << index;
	this->reason = stream.str();
}
const char*Box::IndexError::what()const throw(){
	return reason.c_str();
}

Box::KeyError::KeyError(const char *key){
	this->key = key;
}
const char *Box::KeyError::what()const throw(){
	return key.c_str();
}

Box::TypeError::TypeError(const char *excepted,const char *gived){
	//类型错误
	this->excepted = excepted;
	this->gived = gived;
	std::stringstream stream;
	stream << "excepted type:" <<"'"<< excepted <<"'"<<" gived type:" <<"'"<<gived<<"'";
	this->reason = stream.str();
}
const char *Box::TypeError::what() const throw(){
	return reason.c_str();
}
//空指针错误
const char *Box::NullPtrException::what() const throw(){
	return "Got nullptr";
}
