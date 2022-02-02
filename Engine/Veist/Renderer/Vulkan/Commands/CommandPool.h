#pragma once


#include <vulkan/vulkan.h>

namespace Veist
{

class CommandBuffer;

class CommandPool
{

public:

	CommandPool(int thread_id = 0);

	void resetPool();

	VkCommandPool commandPool() const { return m_pool; };

	CommandBuffer allocateCommandBuffer(bool begin_cmd_buffer = false);

private:

	VkCommandPool m_pool;
	
	std::vector<VkFence> m_fences;//command buffer fences

	uint32_t m_thread_id; //Command pools must be exclusive to each thread NO MULTITHREADING FOR NOW BUT FOR LATER

};

}