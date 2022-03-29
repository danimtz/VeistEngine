#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>


namespace Veist
{

class DescriptorSetLayoutCache
{
public:

	DescriptorSetLayoutCache() = default;

	VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo* info);

	
	struct DescriptorSetLayoutInfo {
		std::vector<VkDescriptorSetLayoutBinding> bindings; //TODO: maybe shouldnt be a vector (dynamic alloc every pushback)

		bool operator==(const DescriptorSetLayoutInfo& other) const;

		size_t hash() const;
	};

private:

	struct DescriptorHash {
		
		size_t operator()(const DescriptorSetLayoutInfo& info) const {
			return info.hash();
		};

	};

	std::unordered_map<DescriptorSetLayoutInfo, VkDescriptorSetLayout, DescriptorHash> m_layouts;
	
};

}