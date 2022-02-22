#pragma once

#include <iostream>

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



