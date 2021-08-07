#pragma once


#include "vulkan/vulkan.h"
#include "Engine/Renderer/Vulkan/Buffers/VulkanBuffer.h"
#include "Engine/Logger.h"




class IndexBuffer 
{
public:

	IndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size);

	VkBuffer getBuffer() const { return m_index_buffer.buffer(); };
	uint32_t getSize() const { return m_index_count; };

private:


	VulkanBuffer		m_index_buffer;
	uint32_t			m_index_count;
	uint32_t			m_index_size; //VK_INDEX_TYPE_UINT16
};