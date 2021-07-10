#include <iostream>
#include <memory>

#include "Renderer/RenderBackend.h"

#include "Platform/Vulkan/RenderBackend_Vulkan.h"

std::unique_ptr<RenderBackend> RenderBackend::CreateBackend(void* window) 
{

	//Could select between vulkan/directx etc backends here. Just vulkan for the forseeable future

	return std::make_unique<RenderBackend_Vulkan>(static_cast<GLFWwindow*>(window));


}