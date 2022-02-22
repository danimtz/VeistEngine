#pragma once

#include "RenderGraphResource.h"

#include <Veist/Graphics/RenderModule.h>



namespace Veist
{
	class RenderGraph;

	class RenderGraphPass
	{
	public:


		using RenderFunction = std::function<void(CommandBuffer&, const RenderGraphPass*)>;


		RenderGraphPass(std::string_view name, RenderGraph* graph);

		const std::string& name() const {return m_name;};


	private:

		friend class RenderGraphPassBuilder;
		

		std::string m_name;

		RenderFunction m_render_function = [](CommandBuffer&, const RenderGraphPass*){};

		RenderGraph* m_graph;

	
	};

}