#include "VulkanBuffer.h"


//NOTE TO SELF: im putting the definition for VMA here although i would like it to be in VulkanRenderBackend.cpp 
//BUT compiler gets very angry if i place it there, probably due to initialization order of other libraries that maybe vma or vulkan depend on (<windows.h> or something the sort)
//FOR now im leaving it here since memory allocator is very related to buffers, but look for a way to put it back in renderbackend later on
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>


//Create vertex buffer
VulkanVertexBuffer::VulkanVertexBuffer( uint32_t size) : m_size(size)
{
	allocateBuffer();
}

VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32_t size) : m_size(size)
{
	
	allocateBuffer();

	//copy vertex data
	VmaAllocator allocator = static_cast<VmaAllocator>(RenderModule::getRenderBackend()->getAllocator()); //could DRY here since im calling allocator in allocateBuffer but for now fine.
	void* data;
	vmaMapMemory(allocator, m_allocation, &data);

	memcpy(data, vertices, size);

	vmaUnmapMemory(allocator, m_allocation);
}

void VulkanVertexBuffer::allocateBuffer() {
	
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = m_size;//size in bytes
	buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vmaalloc_info = {};
	vmaalloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	VmaAllocator allocator = static_cast<VmaAllocator>(RenderModule::getRenderBackend()->getAllocator());

	VK_CHECK(vmaCreateBuffer(allocator, &buffer_create_info, &vmaalloc_info, &m_vertex_buffer, &m_allocation, nullptr));

	//add the destruction of triangle mesh buffer to the deletion queue
	RenderModule::getRenderBackend()->pushToDeletionQueue([=]() { vmaDestroyBuffer(allocator, m_vertex_buffer, m_allocation); });

}