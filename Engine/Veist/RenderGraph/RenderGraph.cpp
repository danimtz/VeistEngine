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
			uint32_t index = m_passes.size();
			m_pass_to_idx_map.emplace(pass->name(), index);
			pass->m_pass_index = index;
		}


		return RenderGraphPassBuilder(pass);

	}


	RenderGraphBufferResource* RenderGraph::getOrAddBufferResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return static_cast<RenderGraphBufferResource*>(m_resources[index].get());
		}
		else
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<RenderGraphBufferResource>(RenderGraphBufferResource(index)));
			
			return static_cast<RenderGraphBufferResource*>(resource.get());
		}
	}
	


	RenderGraphImageResource* RenderGraph::getOrAddImageResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return static_cast<RenderGraphImageResource*>(m_resources[index].get());
		}
		else
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<RenderGraphImageResource>(RenderGraphImageResource(index)));

			return static_cast<RenderGraphImageResource*>(resource.get());
		}
	}


	bool RenderGraph::setOutputBuffer(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;
			m_output_buffer = index;

			return true;
		}
		else
		{
			return false;
		}
	}



	void RenderGraph::execute(CommandBuffer& cmd)
	{

		//Validate graph

		//Allocate physical resources

		//Build descriptors and framebuffers/renderpasses 
		//TODO: materials depend on renderpass, therefore must compile them at runtime (keep a hash map "cache" of already compiled materials so that they arent compiled EVERY frame)

		//Execute rendergraph passes

		//TODO barriers. 




	}



}