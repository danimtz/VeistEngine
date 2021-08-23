#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#include "Engine/Renderer/Vulkan/Buffers/ShaderBuffer.h"

#include "Engine/Renderer/Vulkan/Images/Sampler.h"

#include "Engine/Renderer/RenderModule.h"


//Note: if for example descriptor set 3 is used, the pipeline MUST have descriptors 1 and 2 declared in the shaders.
void DescriptorSet::setDescriptorSetLayout(uint32_t set, const GraphicsPipeline* pipeline)
{
	if (set > pipeline->shaderProgram()->descriptorLayouts().size())
	{
		CRITICAL_ERROR_LOG("Pipeline does not contain descriptor set number used");
	}
	m_descriptor_layout = pipeline->shaderProgram()->descriptorLayouts()[set];
	m_set_number = set;
}


void DescriptorSet::bindSampledTexture(uint32_t binding, const Texture* image, VkDescriptorType type, VkSampler sampler/*sampler view etc?*/)
{
	DescriptorInfo desc_info;
	desc_info.image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	desc_info.image_info.imageView = image->imageView();
	desc_info.image_info.sampler = sampler;


	m_write_data.push_back({ desc_info });
	//m_buffer_infos.push_back({desc_info});

	VkWriteDescriptorSet set_write = {};
	set_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	set_write.pNext = nullptr;
	set_write.dstBinding = binding;
	set_write.descriptorCount = 1;
	set_write.descriptorType = type;


	m_writes.push_back(set_write);
}


void DescriptorSet::bindBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range, VkDescriptorType type)
{
	DescriptorInfo desc_info;
	desc_info.buffer_info.buffer = buffer->buffer();
	desc_info.buffer_info.offset = 0;
	desc_info.buffer_info.range = range;

	m_write_data.push_back({desc_info});
	//m_buffer_infos.push_back({desc_info});

	VkWriteDescriptorSet set_write = {};
	set_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	set_write.pNext = nullptr;
	set_write.dstBinding = binding;
	set_write.descriptorCount = 1;
	set_write.descriptorType = type;

	
	m_writes.push_back(set_write);
}

void DescriptorSet::bindUniformBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range)
{
	bindBuffer(binding, buffer, range, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
}

void DescriptorSet::bindStorageBuffer(uint32_t binding, const ShaderBuffer* buffer, uint32_t range)
{
	bindBuffer(binding, buffer, range, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
}

void DescriptorSet::bindCombinedSamplerTexture(uint32_t binding, const Texture* texture/*sampler view etc?*/)
{
	//Create sampler here TODO
	Sampler sampler = Sampler{ SamplerType::RepeatLinear };

	bindSampledTexture(binding, texture, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler.sampler() /*sampler view etc?*/);
}

void DescriptorSet::updateDescriptorSet()
{

	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	vkUpdateDescriptorSets(device, m_writes.size(), m_writes.data(), 0, nullptr);
	m_writes.clear();
	m_write_data.clear();
};

void DescriptorSet::buildDescriptorSet()
{
	//Allocate descriptor set
	DescriptorSetAllocator* set_allocator = RenderModule::getRenderBackend()->getDescriptorAllocator();

	if (!set_allocator->allocateDescriptorSet(*this)) {
		CRITICAL_ERROR_LOG("Could not allocate descriptor set!");
	}


	//set descriptor set and buffer infos in descriptor writes TODO: what does this for loop do if theres images in descriptor set??
	for (int i = 0; i < m_writes.size(); i++) {
	
		if (m_writes[i].dstBinding != i) {
			CRITICAL_ERROR_LOG("Descriptor write does not match binding: Ensure descriptors are bound in order");
		}

		m_writes[i].pBufferInfo = &m_write_data[i].buffer_info;
		m_writes[i].pImageInfo = &m_write_data[i].image_info;
		m_writes[i].dstSet = m_descriptor_set;
		
	}



	//update descriptor set
	updateDescriptorSet();


}
