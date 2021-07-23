#pragma once

#include "vulkan/vulkan.h"
#include <vk_mem_alloc.h>
#include "Engine/Renderer/Buffers/Buffer.h"
#include "Engine/Renderer/RenderModule.h"
#include "Engine/Logger.h"



class VulkanBuffer
{
public:
	
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);

	VkBuffer buffer() const { return m_buffer; };
	VmaAllocation allocation() const { return m_allocation; };

private:

	void allocateBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);
	
	VkBuffer			m_buffer;
	VmaAllocation		m_allocation;
};



