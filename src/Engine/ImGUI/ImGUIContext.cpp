#include "ImGUIContext.h"
#include "Platform/Vulkan/VulkanImGUIContext.h"


std::unique_ptr<ImGUIContext> ImGUIContext::Create()
{

	//Select platform gui context Vulkan or whatever else
	return std::make_unique<VulkanImGUIContext>();

}