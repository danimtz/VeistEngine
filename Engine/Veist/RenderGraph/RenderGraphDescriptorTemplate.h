#pragma once

#include <Veist/Graphics/RenderModule.h>

namespace Veist
{

	class RenderGraphDescriptorTemplate
	{
	public:

		RenderGraphDescriptorTemplate(uint32_t set_number, uint32_t binding_number, uint32_t resource_index, VkDescriptorType type, SamplerType sampler_type) : isDescriptorBuilt(false),
		m_set_number(set_number), m_binding_number(binding_number), m_resource_index(resource_index), m_descriptor(type), m_sampler_type(sampler_type)
		{}

		//TODO: possbile bug passing reference to descriptor
		RenderGraphDescriptorTemplate(uint32_t set_number, uint32_t binding_number, const Descriptor& descriptor, SamplerType sampler_type = SamplerType::None) :
			isDescriptorBuilt(true), m_set_number(set_number), m_binding_number(binding_number), m_resource_index(-1), m_descriptor(descriptor), m_sampler_type(sampler_type)
		{}

	private:
		bool isDescriptorBuilt;
		uint32_t m_set_number;
		uint32_t m_binding_number;
		int32_t m_resource_index;
		SamplerType m_sampler_type;
		Descriptor m_descriptor; 
	};


}