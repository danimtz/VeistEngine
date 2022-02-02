#include <iostream>



#define CONSOLE_LOG(x)										\
		std::cout << "CONSOLE LOG:\t" << x << std::endl;

#define CRITICAL_ERROR_LOG(x)									\
		std::cout << "CRITICAL ERROR:\t" << x << std::endl;\
		abort();



