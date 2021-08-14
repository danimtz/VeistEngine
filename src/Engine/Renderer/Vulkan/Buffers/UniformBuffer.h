#pragma once


#include "Engine/Renderer/Vulkan/Buffers/Buffer.h"
#include <vulkan/vulkan.h>


#include "Engine/Logger.h"


class UniformBuffer 
{
public:

	UniformBuffer(uint32_t size);
	UniformBuffer(uint32_t subbuffer_size, uint32_t subbuffer_count);

	VkBuffer buffer() const { return m_buffer.buffer(); };
	uint32_t size() const { return m_size; };
	uint32_t offset() const { return m_offset; };

	void setData(const void* data, uint32_t size, uint32_t sub_allocation_num = 0);

private:
	
	VulkanBuffer m_buffer;
	uint32_t m_size;
	uint32_t m_offset{0};
};

