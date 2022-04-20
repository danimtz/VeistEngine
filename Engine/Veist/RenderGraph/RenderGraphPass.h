#pragma once

#include "Resource.h"
#include "DescriptorTemplate.h"

#include <Veist/Graphics/RenderModule.h>
#include <Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h>


namespace Veist
{
namespace RenderGraph
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

		ShaderBuffer* getPhysicalBuffer(BufferResource* resource) const;
		ImageBase* getPhysicalImage(ImageResource* resource) const;

		const std::vector<DescriptorSet>& getDescriptorSets() const {return m_descriptor_sets;};

		void executePass(CommandBuffer& cmd);

	private:
		
		friend class PassBuilder;
		friend class RenderGraph;

		void buildDescriptors();
		void buildFramebuffer();
		

		std::vector<ImageResource*> m_color_outputs;
		std::vector<ImageResource*> m_color_inputs;
		ImageResource* m_depth_output{nullptr};
		ImageResource* m_depth_input{nullptr};


		std::map<uint32_t, std::vector<DescriptorTemplate>> m_descriptor_set_templates;

		std::string m_name;

		RenderFunction m_render_function = [](CommandBuffer&, const RenderGraphPass*){};

		RenderGraph* m_graph;

		uint32_t m_pass_index;


		std::vector<Resource*> m_resource_reads;//Pointers to graph resource inputs, used when building graph
		std::vector<Resource*> m_resource_writes;//Pointers to graph resource outputs, used when building graph

		std::unordered_set<Resource*> m_pass_resources; //Unique list of resources used in pass. used when calculating resource aliasing
		std::unordered_set<PhysicalResource*> m_pass_physical_resources; //Unique list of physical resources used in pass. used when calculating resource barriers
		uint32_t m_resource_write_count; //refcount of resource writes


		Framebuffer m_framebuffer;
		std::vector<DescriptorSet> m_descriptor_sets;

		
		
	};

}
}