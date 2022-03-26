#include "pch.h"
#include "RenderGraphPass.h"
#include "RenderGraphDescriptorTemplate.h"


namespace Veist
{


	RenderGraphPass::RenderGraphPass(std::string_view name, RenderGraph* graph) : m_name(name), m_graph(graph), m_resource_write_count(0) {};



	void RenderGraphPass::addDescriptorTemplate(uint32_t descriptor_set_number, uint32_t resource_index, VkDescriptorType descriptor_type, SamplerType sampler_type)
	{
		auto it = m_descriptor_set_templates.find(descriptor_set_number);
		if (it != m_descriptor_set_templates.end())
		{
			auto binding_number = it->second.size();
			it->second.emplace_back(descriptor_set_number, binding_number, resource_index, descriptor_type, sampler_type);
		}
		else
		{
			auto& desc_set = m_descriptor_set_templates.emplace(descriptor_set_number, std::vector<RenderGraphDescriptorTemplate>()).first->second;
			auto binding_number = desc_set.size();
			desc_set.emplace_back(descriptor_set_number, binding_number, resource_index, descriptor_type, sampler_type);
		}
	}


	void RenderGraphPass::addExternalDescriptorTemplate(uint32_t descriptor_set_number, const Descriptor& descriptor)
	{
		auto it = m_descriptor_set_templates.find(descriptor_set_number);
		if (it != m_descriptor_set_templates.end())
		{
			auto binding_number = it->second.size();
			it->second.emplace_back(descriptor_set_number, binding_number, descriptor);
		}
		else
		{
			auto& desc_set = m_descriptor_set_templates.emplace(descriptor_set_number, std::vector<RenderGraphDescriptorTemplate>()).first->second;
			auto binding_number = desc_set.size();
			desc_set.emplace_back(descriptor_set_number, binding_number, descriptor);
		}
	}

	/*ShaderBuffer* RenderGraphPass::getPhysicalBuffer(RenderGraphBufferResource* resource) const
	{
		return nullptr;
	}

	ImageBase* RenderGraphPass::getPhysicalImage(RenderGraphImageResource* resource) const
	{
		return nullptr;
	}*/




}