#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include "Engine/Logger.h"

class DescriptorSetLayoutCache
{
public:


	VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo* info);

	

private:

	std::unordered_map<uint32_t, VkDescriptorSetLayout> m_layouts;
	
};

