#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSetLayoutCache.h"

class DescriptorSetAllocator
{
public:
	
	DescriptorSetAllocator();


	bool allocateDescriptorSet(DescriptorSet& descriptor_set){};


	DescriptorSetLayoutCache* layoutCache() { return m_layout_cache.get(); };

private:

	std::vector<VkDescriptorPool> m_pools;

	std::unique_ptr<DescriptorSetLayoutCache> m_layout_cache;
	
};

