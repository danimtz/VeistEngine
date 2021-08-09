#pragma once


#include "Engine/Renderer/Vulkan/Buffers/Buffer.h"
#include <vulkan/vulkan.h>


#include "Engine/Logger.h"


class UniformBuffer 
{
public:

	UniformBuffer(uint32_t size);

	VkBuffer getBuffer() const { return m_buffer.buffer(); };
	uint32_t getSize() const { return m_size; };

	void setData(const void* data, uint32_t size, uint32_t offset = 0);

private:
	
	VulkanBuffer m_buffer;
	uint32_t m_size;

};

