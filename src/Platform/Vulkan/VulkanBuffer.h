#pragma once

#include "vulkan/vulkan.h"
#include <vk_mem_alloc.h>
#include "Renderer/Buffer.h"
#include "Renderer/RenderModule.h"
#include "Logger.h"


class VulkanVertexBuffer : public VertexBuffer
{
public:

	VulkanVertexBuffer(uint32_t size);
	VulkanVertexBuffer(void* vertices, uint32_t size);


	virtual void* getBuffer() const override { return m_vertex_buffer; };
	virtual uint32_t getSize() const override { return m_size; };

	//virtual void setInputDescription(const VertexDescription &description) override {};
	

private:

	void allocateBuffer();

	VkBuffer		m_vertex_buffer;
	VmaAllocation	m_allocation;
	uint32_t		m_size;
};

