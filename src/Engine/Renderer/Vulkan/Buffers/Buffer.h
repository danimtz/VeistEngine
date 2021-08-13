#pragma once

#include <vk_mem_alloc.h>

#include "Engine/Logger.h"



class VulkanBuffer
{
public:
	
	VulkanBuffer() = default;
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);

	VkBuffer buffer() const { return m_buffer; };
	VmaAllocation allocation() const { return m_allocation; };

private:

	void allocateBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);
	
	VkBuffer			m_buffer;
	VmaAllocation		m_allocation;
};



