#pragma once

#include "vulkan/vulkan.h"
#include "Engine/Renderer/Vulkan/Buffers/Buffer.h"
#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Logger.h"



class StagingBuffer 
{
public:

	StagingBuffer(void* data, uint32_t size);
	~StagingBuffer();

	VkBuffer getBuffer() const  { return m_staging_buffer.buffer(); };
	virtual uint32_t getSize() const  { return m_size; };

	
private:

	VulkanBuffer		m_staging_buffer;
	uint32_t			m_size;

};
