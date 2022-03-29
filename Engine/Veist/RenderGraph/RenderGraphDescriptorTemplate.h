#pragma once

#include <Veist/Graphics/RenderModule.h>

namespace Veist
{

	struct RenderGraphDescriptorTemplate
	{
	public:

		RenderGraphDescriptorTemplate(uint32_t set_number, uint32_t binding_number, uint32_t resource_index, VkDescriptorType type, SamplerType sampler_type) :
			m_set_number(set_number), m_binding_number(binding_number), m_resource_index(resource_index), m_descriptor_type(type), m_sampler_type(sampler_type) {}

		//TODO: possbile bug passing reference to descriptor, or outright remove this one we'll see
		RenderGraphDescriptorTemplate(uint32_t set_number, uint32_t binding_number, const Descriptor& descriptor) :
		 m_set_number(set_number), m_binding_number(binding_number), m_resource_index(-1), m_sampler_type(SamplerType::None),
		is_external_descriptor(true), m_external_descriptor(std::make_shared<Descriptor>(descriptor))	
		{}


		//uint32_t resourceIndex() const {return m_resource_index;};
		
	private:
		friend class RenderGraphPass;

		uint32_t m_set_number;
		uint32_t m_binding_number;
		int32_t m_resource_index;
		VkDescriptorType m_descriptor_type;
		SamplerType m_sampler_type;


		bool is_external_descriptor{false};
		std::shared_ptr<Descriptor> m_external_descriptor{nullptr}; 

		
	};


}