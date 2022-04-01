#pragma once

#include <vk_mem_alloc.h>




namespace Veist
{


class VulkanBuffer
{
public:
	
	VulkanBuffer() = default;
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);
	virtual ~VulkanBuffer();

	VulkanBuffer(VulkanBuffer&&);
	VulkanBuffer& operator=(VulkanBuffer&&);


	VkBuffer buffer() const { return m_buffer; };
	VmaAllocation allocation() const { return m_allocation; };

	

private:

	void allocateBuffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);
	
	VkBuffer			m_buffer;
	VmaAllocation		m_allocation;
	
};



class Buffer
{
public:

	Buffer() = default;
	Buffer(uint32_t size, VkBufferUsageFlagBits buffer_usage, VmaMemoryUsage memory_usage);

	VkBuffer getBuffer() const { return m_buffer.buffer(); };
	uint32_t getSize() const { return m_size; };

protected:

	VulkanBuffer		m_buffer;
	uint32_t			m_size{ 0 };

};

}