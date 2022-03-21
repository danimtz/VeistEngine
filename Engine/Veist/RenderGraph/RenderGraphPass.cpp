#include "pch.h"
#include "RenderGraphPass.h"



namespace Veist
{


	RenderGraphPass::RenderGraphPass(std::string_view name, RenderGraph* graph) : m_name(name), m_graph(graph) {};



	/*ShaderBuffer* RenderGraphPass::getPhysicalBuffer(RenderGraphBufferResource* resource) const
	{
		return nullptr;
	}

	ImageBase* RenderGraphPass::getPhysicalImage(RenderGraphImageResource* resource) const
	{
		return nullptr;
	}*/



}