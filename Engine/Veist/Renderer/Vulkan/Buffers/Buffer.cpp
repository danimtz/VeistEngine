#include "pch.h"
#include "Veist/Renderer/Vulkan/Buffers/Buffer.h"

#include "Veist/Renderer/RenderModule.h"

namespace Veist
{


Buffer::Buffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage) : m_size(size), m_buffer({size, buffer_usage, memory_usage}) 
{
}



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

	//add the destruction of triangle mesh buffer to the deletion queue (Dont add for staging buffer as these will be deleted in destructor)
	if (buffer_usage != VK_BUFFER_USAGE_TRANSFER_SRC_BIT){
		RenderModule::getRenderBackend()->pushToDeletionQueue([allocator, buffer, allocation]() { vmaDestroyBuffer(allocator, buffer, allocation); });
	}
	
}

}