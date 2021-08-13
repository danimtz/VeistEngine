#pragma once


#include <vulkan/vulkan.h>
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"

class UniformBuffer;

class DescriptorSet
{
public:
	
	DescriptorSet() = default;
	void setDescriptorSetLayout(uint32_t set, const GraphicsPipeline* pipeline);
	void bindUniformBuffer(uint32_t binding, const UniformBuffer* buffer, uint32_t range);
	
	void buildDescriptorSet();
	void updateDescriptorSet();

	VkDescriptorSet descriptorSet() const { return m_descriptor_set; };
	
private:

	VkDescriptorSetLayout m_descriptor_layout = nullptr;
	VkDescriptorSet m_descriptor_set = nullptr;

	std::vector<VkWriteDescriptorSet> m_writes;

};

