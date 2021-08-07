#include "Engine/Renderer/Vulkan/Buffers/VulkanIndexBuffer.h"
#include "Engine/Renderer/RenderModule.h"



IndexBuffer::IndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size) : m_index_count(idx_count), m_index_size(index_size),
	m_index_buffer( { idx_count * index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU } )
{

	//copy vertex data
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();
	void* data;
	vmaMapMemory(allocator, m_index_buffer.allocation(), &data);

	memcpy(data, indices, m_index_count * m_index_size);

	vmaUnmapMemory(allocator, m_index_buffer.allocation()); //THIS GIVES ERROPRS SOMETIMES
}
