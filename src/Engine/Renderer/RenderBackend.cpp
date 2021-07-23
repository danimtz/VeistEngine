
#include "Engine/Renderer/RenderBackend.h"

#include "Platform/Vulkan/VulkanRenderBackend.h"

std::shared_ptr<RenderBackend> RenderBackend::CreateBackend() 
{

	//Could select between vulkan/directx etc backends here. Just vulkan for the forseeable future

	return std::make_shared<VulkanRenderBackend>();


}