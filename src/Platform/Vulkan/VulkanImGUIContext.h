#pragma once

#include "vulkan/vulkan.h"

#include "Engine/ImGUI/ImGUIContext.h"

class VulkanImGUIContext : public ImGUIContext
{
public:
	
	VulkanImGUIContext();

private:
	
	void initImGUI();

};

