#include "VulkanVertexBuffer.h"




VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32_t size) : m_size(size), m_vertex_buffer({ size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU })
{

	//copy vertex data
	VmaAllocator allocator = static_cast<VmaAllocator>(RenderModule::getRenderBackend()->getAllocator()); //could DRY here since im calling allocator in allocateBuffer but for now fine.
	void* data;
	vmaMapMemory(allocator, m_vertex_buffer.allocation(), &data);

	memcpy(data, vertices, size);

	vmaUnmapMemory(allocator, m_vertex_buffer.allocation());
}

