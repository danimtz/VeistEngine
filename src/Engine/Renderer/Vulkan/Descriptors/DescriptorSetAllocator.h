#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSetLayoutCache.h"

class DescriptorSetAllocator
{
public:
	
	DescriptorSetAllocator();

	void cleanup();

	bool allocateDescriptorSet(DescriptorSet& descriptor_set);

	void resetPools();

	DescriptorSetLayoutCache* layoutCache() { return m_layout_cache.get(); };


	//from vkguide.dev (should edit it later to fit my needs exactly)
	struct PoolSizes {
		std::vector<std::pair<VkDescriptorType, float>> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
		};
	};

private:

	VkDescriptorPool getPool();

	PoolSizes m_descriptor_sizes;
	VkDescriptorPool m_current_pool{nullptr};

	std::vector<VkDescriptorPool> m_free_pools;
	std::vector<VkDescriptorPool> m_used_pools;
	std::unique_ptr<DescriptorSetLayoutCache> m_layout_cache;
	
};

