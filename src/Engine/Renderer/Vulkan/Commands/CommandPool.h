#pragma once


#include <vulkan/vulkan.h>

class CommandPool
{

public:

	CommandPool(int thread_id = 0);

	void resetPool();

	VkCommandPool commandPool() const { return m_pool; };


private:

	VkCommandPool m_pool;


	uint32_t m_thread_id; //Command pools must be exclusive to each thread NO MULTITHREADING FOR NOW BUT FOR LATER
};

