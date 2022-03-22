#include "pch.h"
#include "RenderGraphPass.h"



namespace Veist
{


	RenderGraphPass::RenderGraphPass(std::string_view name, RenderGraph* graph) : m_name(name), m_graph(graph) {};



	void RenderGraphPass::addDescriptorTemplate(uint32_t descriptor_set_number,  uint32_t resource_index)
	{
		auto it = m_descriptor_set_templates.find(descriptor_set_number);
		if (it != m_descriptor_set_templates.end())
		{
			it->second.emplace_back(RenderGraphDescriptorTemplate());
		}
		else
		{
			auto& desc_set = m_descriptor_set_templates.emplace(descriptor_set_number, std::vector<Foo>());
			desc_set.first->second.emplace_back(RenderGraphDescriptorTemplate());


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