#pragma once

#include "vulkan/vulkan.h"
#include "Platform/Vulkan/Buffers/VulkanBuffer.h"
#include "Engine/Renderer/RenderModule.h"
#include "Engine/Logger.h"



class VulkanVertexBuffer : public VertexBuffer
{
public:

	VulkanVertexBuffer(void* vertices, uint32_t size);


	virtual void* getBuffer() const override { return m_vertex_buffer.buffer(); };
	virtual uint32_t getSize() const override { return m_size; };

	virtual const VertexDescription& getInputDescription() const override { return m_description; }
	virtual void setInputDescription(const VertexDescription& description) override { m_description = description; };


private:

	VulkanBuffer		m_vertex_buffer;
	VertexDescription	m_description;
	uint32_t			m_size;

};
