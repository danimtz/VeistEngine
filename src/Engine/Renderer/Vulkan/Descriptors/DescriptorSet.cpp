#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#include "Engine/Renderer/Vulkan/Buffers/UniformBuffer.h"
#include "Engine/Renderer/RenderModule.h"


void DescriptorSet::setDescriptorSetLayout(uint32_t set, const GraphicsPipeline* pipeline)
{
	m_descriptor_layout = pipeline->shaderProgram()->descriptorLayouts()[0];
	m_set_number = set;
}


void DescriptorSet::bindUniformBuffer(uint32_t binding, const UniformBuffer* buffer, uint32_t range)
{
	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = buffer->buffer();
	buffer_info.offset = 0;
	buffer_info.range = range;
	m_buffer_infos.push_back(buffer_info);

	VkWriteDescriptorSet set_write = {};
	set_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	set_write.pNext = nullptr;
	set_write.dstBinding = binding;
	set_write.descriptorCount = 1;
	set_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	

	m_writes.push_back(set_write);

}


void DescriptorSet::updateDescriptorSet()
{

	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	vkUpdateDescriptorSets(device, m_writes.size(), m_writes.data(), 0, nullptr);
	m_writes.clear();
};

void DescriptorSet::buildDescriptorSet()
{
	//Allocate descriptor set
	//TODO
	DescriptorSetAllocator* set_allocator = RenderModule::getRenderBackend()->getDescriptorAllocator();

	if (!set_allocator->allocateDescriptorSet(*this)) {
		CRITICAL_ERROR_LOG("Could not allocate descriptor set!");
	}


	//set descriptor set and buffer infos in descriptor writes
	for (int i = 0; i < m_writes.size(); i++) {
		m_writes[i].dstSet = m_descriptor_set;
		m_writes[i].pBufferInfo = &m_buffer_infos[i];
	}



	//update descriptor set
	updateDescriptorSet();


}
