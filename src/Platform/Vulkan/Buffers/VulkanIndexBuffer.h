#pragma once


#include "vulkan/vulkan.h"
#include "Platform/Vulkan/Buffers/VulkanBuffer.h"
#include "Engine/Renderer/RenderModule.h"
#include "Engine/Logger.h"




class VulkanIndexBuffer : public IndexBuffer
{
public:

	VulkanIndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size);

	virtual void* getBuffer() const override { return m_index_buffer.buffer(); };
	virtual uint32_t getSize() const override { return m_index_count; };

private:


	VulkanBuffer		m_index_buffer;
	uint32_t			m_index_count;
	uint32_t			m_index_size; //VK_INDEX_TYPE_UINT16
};