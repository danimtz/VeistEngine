#include "Buffer.h"
#include "Platform/Vulkan/VulkanBuffer.h"


std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
{
	
	//Select platform buffer Vulkan or whatever else
	return std::make_shared<VulkanVertexBuffer>(vertices, size);

}

std::shared_ptr<VertexBuffer> VertexBuffer::Create( uint32_t size)
{

	//Select platform buffer Vulkan or whatever else
	return std::make_shared<VulkanVertexBuffer>(size);

}