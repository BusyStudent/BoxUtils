#ifndef _BOXUTILS_RANDOM_HPP_
#define _BOXUTILS_RANDOM_HPP_
#include <ctime>
namespace BoxUtils{
	class Random{
		public:
			Random();
			void set_seed(unsigned int seed = (unsigned int)(time(nullptr)));
			int randint(int begin,int end);
			unsigned int get_seed();
		private:
			unsigned int seed;
	};
};
#endif
