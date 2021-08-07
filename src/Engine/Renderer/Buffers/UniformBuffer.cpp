#include "UniformBuffer.h"
#include "Platform/Vulkan/Buffers/VulkanUniformBuffer.h"

std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size)
{

	//Could select between vulkan/directx etc backends here. Just vulkan for the forseeable future

	return std::make_shared<VulkanUniformBuffer>(size);


}