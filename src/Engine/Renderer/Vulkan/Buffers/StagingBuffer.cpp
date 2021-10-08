#include "Engine/Renderer/Vulkan/Buffers/StagingBuffer.h"
#include "Engine/Renderer/RenderModule.h"



StagingBuffer::StagingBuffer(void* data, uint32_t size) : Buffer({ size, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT) , VMA_MEMORY_USAGE_CPU_ONLY })
{

	

	//allocate the buffer
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();

	void* dst_data;
	vmaMapMemory(allocator, m_buffer.allocation(), &dst_data);

	memcpy(dst_data, data, static_cast<size_t>(size));

	vmaUnmapMemory(allocator, m_buffer.allocation());
	
}

StagingBuffer::~StagingBuffer()
{
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();
	vmaDestroyBuffer(allocator, m_buffer.buffer(), m_buffer.allocation());
}