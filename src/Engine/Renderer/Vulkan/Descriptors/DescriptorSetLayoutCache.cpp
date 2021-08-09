#include "DescriptorSetLayoutCache.h"



VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo* info)
{
	
	//TODO: Search layout cache to see if that descriptor exists already

	VkDescriptorSetLayout layout;
	VK_CHECK(vkCreateDescriptorSetLayout(device, info, nullptr, &layout));

	return layout;

};

