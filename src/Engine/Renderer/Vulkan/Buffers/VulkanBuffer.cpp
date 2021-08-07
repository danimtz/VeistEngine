#include "VulkanBuffer.h"

#include "Engine/Renderer/RenderModule.h"


//NOTE TO SELF: im putting the definition for VMA here although i would like it to be in VulkanRenderBackend.cpp 
//BUT compiler gets very angry if i place it there, probably due to initialization order of other libraries that maybe vma or vulkan depend on (<windows.h> or something the sort)
//FOR now im leaving it here since memory allocator is very related to buffers, but look for a way to put it back in renderbackend later on




//Create vertex buffer
/*VulkanVertexBuffer::VulkanVertexBuffer( uint32_t size) : m_size(size)
{
	allocateBuffer();
}*/



VulkanBuffer::VulkanBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage)
{
	
	allocateBuffer(size, buffer_usage, memory_usage);

}

void VulkanBuffer::allocateBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage)

{
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;//size in bytes
	buffer_create_info.usage = buffer_usage;


	VmaAllocationCreateInfo vma_alloc_info = {};
	vma_alloc_info.usage = memory_usage;

	//allocate the buffer
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();

	VkBuffer buffer;
	VmaAllocation allocation;
	VK_CHECK(vmaCreateBuffer(allocator, &buffer_create_info, &vma_alloc_info, &buffer, &allocation, nullptr));

	m_allocation = allocation;
	m_buffer = buffer;

	//add the destruction of triangle mesh buffer to the deletion queue
	RenderModule::getRenderBackend()->pushToDeletionQueue([allocator, buffer, allocation]() { vmaDestroyBuffer(allocator, buffer, allocation); });

	
}
