#include <iostream>

#define CONSOLE_LOG(x)										\
		std::string msg = x;								\
		std::cout << "CONSOLE LOG:\t" << msg << std::endl;

#define CRITICAL_ERROR_LOG(x)										\
		std::string msg = x;								\
		std::cout << "CRITICAL ERROR:\t" << msg << std::endl;\
		abort();