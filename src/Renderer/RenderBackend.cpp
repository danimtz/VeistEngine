#include <iostream>
#include <memory>

#include "Renderer/RenderBackend.h"

#include "Platform/Vulkan/RenderBackend_Vulkan.h"

std::shared_ptr<RenderBackend> RenderBackend::CreateBackend() 
{

	//Could select between vulkan/directx etc backends here. Just vulkan for the forseeable future

	return std::make_shared<RenderBackend_Vulkan>();


}