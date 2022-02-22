#include "pch.h"
#include "RenderGraph.h"

//#include "RenderGraphPassBuilder.h"


namespace Veist
{


	RenderGraphPassBuilder RenderGraph::addPass(std::string_view name)
	{

		m_passes.emplace_back(std::make_unique<RenderGraphPass>(name, this));

		RenderGraphPass* pass = m_passes.back().get();

		auto it = m_pass_to_idx_map.find(pass->name());
		if (it != m_pass_to_idx_map.end())
		{
			CRITICAL_ERROR_LOG("Cannot create two identically named rendergraph passes")
		}
		else
		{
			m_pass_to_idx_map.emplace(pass->name(), m_passes.size());
		}



		return RenderGraphPassBuilder(pass);

	}







}