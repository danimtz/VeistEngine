#pragma once


#include <vulkan/vulkan.h>

class UniformBuffer;

class DescriptorSet
{
public:

	DescriptorSet(){};

	void bindUniformBuffer(UniformBuffer& buffer, uint32_t binding){};
	void updateDescriptorSet(){};

private:

	VkDescriptorSetLayout m_descriptor_layout;
	VkDescriptorSet m_descriptor_set;

};

