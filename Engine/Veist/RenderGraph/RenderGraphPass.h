#pragma once

#include "RenderGraphResource.h"

#include <Veist/Graphics/RenderModule.h>
#include <Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h>


namespace Veist
{
	class RenderGraph;

	class RenderGraphPass
	{
	public:


		using RenderFunction = std::function<void(CommandBuffer&, const RenderGraphPass*)>;


		RenderGraphPass(std::string_view name, RenderGraph* graph);

		const std::string& name() const {return m_name;};


		ShaderBuffer* getPhysicalBuffer(RenderGraphBufferResource* resource) const;
		ImageBase* getPhysicalImage(RenderGraphImageResource* resource) const;

	private:

		friend class RenderGraphPassBuilder;
		

		//TODO add storage of rendergraphPass resources !!!! but do i actually need this here?
		//std::vector<RenderGraphTransientImage* > color_outputs;
		//std::vector<RenderGraphTransientBuffer* > storage_outputs;
		//add more for all things in a renderpass etc



		//std::vector<DescriptorSet>

		std::string m_name;

		RenderFunction m_render_function = [](CommandBuffer&, const RenderGraphPass*){};

		RenderGraph* m_graph;

	
	};

}