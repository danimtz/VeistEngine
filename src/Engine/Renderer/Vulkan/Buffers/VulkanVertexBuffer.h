#pragma once

#include "vulkan/vulkan.h"
#include "Engine/Renderer/Vulkan/Buffers/VulkanBuffer.h"
#include "Engine/Renderer/Buffers/VertexDescription.h"
#include "Engine/Logger.h"



class VertexBuffer 
{
public:

	VertexBuffer(void* vertices, uint32_t size);


	VkBuffer getBuffer() const  { return m_vertex_buffer.buffer(); };
	virtual uint32_t getSize() const  { return m_size; };

	const VertexDescription& getInputDescription() const  { return m_description; }
	void setInputDescription(const VertexDescription& description) { m_description = description; };


private:

	VulkanBuffer		m_vertex_buffer;
	VertexDescription	m_description;
	uint32_t			m_size;

};
