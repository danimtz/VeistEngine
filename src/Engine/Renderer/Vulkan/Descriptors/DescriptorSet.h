#pragma once


#include <vulkan/vulkan.h>
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/ComputePipeline.h"
#include "Engine/Renderer/Vulkan/Images/Image.h"

class ShaderBuffer;


class DescriptorSet
{
public:
	
	DescriptorSet() = default;
	void setDescriptorSetLayout(uint32_t set, const GraphicsPipeline* pipeline);
	void setDescriptorSetLayout(uint32_t set, const ComputePipeline* pipeline); //TODO: maybe make compute and graphics pipeline inherit from pipeline? maybe not though

	void bindUniformBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range);
	void bindStorageBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range);
	void bindCombinedSamplerTexture(uint32_t binding, const Texture* texture/*sampler view etc?*/);//TODO combine these two functions to use ImageBase as argument
	void bindCombinedSamplerCubemap(uint32_t binding, const Cubemap* cubemap/*sampler view etc?*/);
	void bindStorageImage(uint32_t binding, const ImageBase* image);
	void bindStorageImage(uint32_t binding, const VkImageView image_view);

	void buildDescriptorSet();
	void updateDescriptorSet();

	uint32_t setNumber() const { return m_set_number; };

	VkDescriptorSet& descriptorSet() { return m_descriptor_set; };
	const VkDescriptorSet& descriptorSet() const { return m_descriptor_set; };
	

	VkDescriptorSetLayout descriptorSetLayout() const { return m_descriptor_layout; };

private:
	void bindBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range, VkDescriptorType type);
	void bindSampledImage(uint32_t binding, VkImageView image_view, VkDescriptorType type, VkSampler sampler);
	

	uint32_t m_set_number;

	VkDescriptorSetLayout m_descriptor_layout = nullptr;
	VkDescriptorSet m_descriptor_set = nullptr;
	
	std::vector<VkWriteDescriptorSet> m_writes;

	//change this to a map
	struct DescriptorInfo {
		VkDescriptorBufferInfo buffer_info = {};
		VkDescriptorImageInfo image_info = {};
	};

	std::vector<DescriptorInfo> m_write_data;
};

