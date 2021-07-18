#pragma once

#include "vulkan/vulkan.h"
#include <vk_mem_alloc.h>
#include "Renderer/Buffer.h"
#include "Renderer/RenderModule.h"
#include "Logger.h"


class VulkanVertexBuffer : public VertexBuffer
{
public:

	//VulkanVertexBuffer(uint32_t size);
	VulkanVertexBuffer(void* vertices, uint32_t size);


	virtual void* getBuffer() const override { return m_vertex_buffer; };
	virtual uint32_t getSize() const override { return m_size; };

	virtual const VertexDescription& getInputDescription() const override { return m_description; }
	virtual void setInputDescription( const VertexDescription &description) override { m_description = description; };
	

private:

	void allocateBuffer();

	VkBuffer			m_vertex_buffer;
	VmaAllocation		m_allocation;
	VertexDescription	m_description;
	uint32_t			m_size;
};


class VulkanIndexBuffer : public IndexBuffer
{
public:

	VulkanIndexBuffer(void* indices, uint32_t idx_count){};


	virtual void* getBuffer() const override { return m_index_buffer; };
	virtual uint32_t getSize() const override { return m_index_count; };

	

private:

	void allocateBuffer(){};

	VkBuffer			m_index_buffer;
	VmaAllocation		m_allocation;
	uint32_t			m_index_count;
};
