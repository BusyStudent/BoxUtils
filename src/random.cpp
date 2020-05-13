#include <ctime>
#include "random.hpp"
Box::Random::Random(){
	this->seed = (unsigned int) time(nullptr);
}
void Box::Random::set_seed(unsigned int seed){
	this->seed = seed;
}
int Box::Random::randint(int begin,int end){
	(this->seed) = (this->seed) *1103515245 +12345;
	int number = (unsigned int )((this->seed)/65536) % 32768;
	number = number % (end + 1);
	if(number < begin){
		return number + begin;
	}
	else{
		return number;
	}
}
unsigned int Box::Random::get_seed(){
	return this->seed;
}
