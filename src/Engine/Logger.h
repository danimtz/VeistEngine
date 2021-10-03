#include <iostream>


#define CONSOLE_LOG(x)										\
		std::cout << "CONSOLE LOG:\t" << x << std::endl;

#define CRITICAL_ERROR_LOG(x)									\
		std::cout << "CRITICAL ERROR:\t" << x << std::endl;\
		abort();



//Check VkResult macro. this can be replaced by error message/crash dump or throw exception. should be in other file
#define VK_CHECK(x)														\
	do																	\
	{																	\
		VkResult err = x;												\
		if (err)														\
		{																\
			std::cout <<"Detected Vulkan error: " << err << ": " << VkResult(err)<<std::endl;	\
			abort();													\
		}																\
	} while (0)

