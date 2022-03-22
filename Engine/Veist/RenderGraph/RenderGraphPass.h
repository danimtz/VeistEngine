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


		void addDescriptorTemplate(uint32_t descriptor_set_number, uint32_t resource_index);

		ShaderBuffer* getPhysicalBuffer(RenderGraphBufferResource* resource) const;
		ImageBase* getPhysicalImage(RenderGraphImageResource* resource) const;

	private:

		friend class RenderGraphPassBuilder;
		friend class RenderGraph;

		//TODO add storage of rendergraphPass resources !!!! but do i actually need this here?
		//std::vector<RenderGraphTransientImage* > color_outputs;
		//std::vector<RenderGraphTransientBuffer* > storage_outputs;
		//add more for all things in a renderpass etc



		std::map<uint32_t, std::vector<RenderGraphDescriptorTemplate>> m_descriptor_set_templates;

		std::string m_name;

		RenderFunction m_render_function = [](CommandBuffer&, const RenderGraphPass*){};

		RenderGraph* m_graph;

		uint32_t m_pass_index;

	
	};

}