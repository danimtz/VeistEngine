#pragma once


#include <vulkan/vulkan.h>

class UniformBuffer;

class DescriptorSet
{
public:
	
	DescriptorSet(){};
	void bindUniformBuffer( const UniformBuffer& buffer, uint32_t binding, uint32_t buffer_size ) {};
	void bindUniformBuffer( const UniformBuffer& buffer, const char* name ) {};
	void updateDescriptorSet(){};
	
private:

	VkDescriptorSetLayout m_descriptor_layout;
	VkDescriptorSet m_descriptor_set;

};

