#include "Buffer.h"
#include "Platform/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Platform/Vulkan/Buffers/VulkanIndexBuffer.h"

std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
{
	
	//Select platform buffer Vulkan or whatever else
	return std::make_shared<VulkanVertexBuffer>(vertices, size);

}

/*
std::shared_ptr<VertexBuffer> VertexBuffer::Create( uint32_t size)
{

	//Select platform buffer Vulkan or whatever else
	return std::make_shared<VulkanVertexBuffer>(size);

}*/


std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t idx_count, uint32_t index_size)
{

	//Select platform buffer Vulkan or whatever else
	return std::make_shared<VulkanIndexBuffer>(indices, idx_count, index_size);

}