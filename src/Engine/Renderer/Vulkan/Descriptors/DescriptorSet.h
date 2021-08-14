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

	uint32_t setNumber() const { return m_set_number; };
	VkDescriptorSet& descriptorSet() { return m_descriptor_set; };
	VkDescriptorSetLayout descriptorSetLayout() const { return m_descriptor_layout; };

private:
	uint32_t m_set_number;

	VkDescriptorSetLayout m_descriptor_layout = nullptr;
	VkDescriptorSet m_descriptor_set = nullptr;
	
	std::vector<VkWriteDescriptorSet> m_writes;
	std::vector<VkDescriptorBufferInfo> m_buffer_infos; //this could be elsewhere maybe?
};

