#pragma once


#include <vulkan/vulkan.h>

class CommandPool
{

public:

	CommandPool() = default;


	VkCommandPool commandPool() const { return m_pool; };

private:

	VkCommandPool m_pool;

};

