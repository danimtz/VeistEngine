#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphDescriptorTemplate.h"

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


		void addDescriptorTemplate(uint32_t descriptor_set_number, uint32_t resource_index, VkDescriptorType descriptor_type, SamplerType sampler_type = SamplerType::None);
		void addExternalDescriptorTemplate(uint32_t descriptor_set_number,  const Descriptor& descriptor);

		ShaderBuffer* getPhysicalBuffer(RenderGraphResource* resource) const {return nullptr; }; //TODO
		ImageBase* getPhysicalImage(RenderGraphResource* resource) const { return nullptr; }; //TODO
		
		void executePass(CommandBuffer& cmd);

	private:
		
		friend class RenderGraphPassBuilder;
		friend class RenderGraph;

		void buildDescriptors();
		void buildFramebuffer();
		
		std::vector<RenderGraphResource*> m_resource_reads;//Pointers to graph resource inputs, used when building graph
		std::vector<RenderGraphResource*> m_resource_writes;//Pointers to graph resource outputs, used when building graph
		uint32_t m_resource_write_count; //refcount of resource writes


		std::vector<RenderGraphImageResource*> m_color_outputs;
		RenderGraphImageResource* m_depth_output;

		std::map<uint32_t, std::vector<RenderGraphDescriptorTemplate>> m_descriptor_set_templates;

		std::string m_name;

		RenderFunction m_render_function = [](CommandBuffer&, const RenderGraphPass*){};

		RenderGraph* m_graph;

		uint32_t m_pass_index;

		Framebuffer m_framebuffer;
		std::vector<DescriptorSet> m_descriptor_sets;

	
	};

}