#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSetLayoutCache.h"
#include "Engine/Renderer/RenderModule.h"


VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo* info)
{
	
	

	//Fill DescriptorLayoutInfo
	DescriptorSetLayoutInfo layout_info;

	//Assumes binding are ordered in ascending order (they should be from shader reflection)
	layout_info.bindings.reserve(info->bindingCount);
	for (int i = 0; i < info->bindingCount; i++) {

		layout_info.bindings.push_back(info->pBindings[i]);
	}


	//Search layout cache to see if that descriptor exists already else create it
	auto it = m_layouts.find(layout_info);
	if (it != m_layouts.end()) {
		return (*it).second;
	}
	else {

		VkDescriptorSetLayout layout;
		VK_CHECK(vkCreateDescriptorSetLayout(device, info, nullptr, &layout));
		RenderModule::getRenderBackend()->pushToDeletionQueue([device, layout]() {vkDestroyDescriptorSetLayout(device, layout, nullptr); });

		m_layouts[layout_info] = layout; //add it to map
		return layout;
	}

};




bool DescriptorSetLayoutCache::DescriptorSetLayoutInfo::operator==(const DescriptorSetLayoutInfo& other) const {
	
	if (bindings.size() != other.bindings.size()) {
		return false;
	} 
	else {
		//Compare each binding
		for (int i = 0; i < bindings.size(); i++) {

			if (bindings[i].binding != other.bindings[i].binding) {
				return false;
			}
			if (bindings[i].descriptorType != other.bindings[i].descriptorType) {
				return false;
			}
			if (bindings[i].descriptorCount != other.bindings[i].descriptorCount) {
				return false;
			}
			if (bindings[i].stageFlags != other.bindings[i].stageFlags) {
				return false;
			}
		}
		return true;
	}
}

//From vkguide.dev
size_t DescriptorSetLayoutCache::DescriptorSetLayoutInfo::hash() const {
	
	size_t result = std::hash<size_t>()(bindings.size());

	for (const VkDescriptorSetLayoutBinding& b : bindings)
	{
		//pack the binding data into a single int64. Not fully correct but it's ok
		size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

		//shuffle the packed binding data and xor it with the main hash. Could also be m * H(A) + H(B) etc
		result ^= std::hash<size_t>()(binding_hash);
	}

	return result;
}
