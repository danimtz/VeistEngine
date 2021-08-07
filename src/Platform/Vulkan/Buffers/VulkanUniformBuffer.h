#pragma once

#include "vulkan/vulkan.h"
#include "Platform/Vulkan/Buffers/VulkanBuffer.h"
#include "Engine/Renderer/RenderModule.h"
#include "Engine/Logger.h"


class VulkanUniformBuffer : public UniformBuffer
{
public:

	VulkanUniformBuffer(uint32_t size);

	virtual void* getBuffer() const override { return m_buffer.buffer(); };
	virtual uint32_t getSize() const override { return m_size; };

	virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) override;

private:
	
	VulkanBuffer m_buffer;
	uint32_t m_size;

};

