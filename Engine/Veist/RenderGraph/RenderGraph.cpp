#include "pch.h"
#include "RenderGraph.h"

//#include "RenderGraphPassBuilder.h"


namespace Veist
{


	RenderGraphPassBuilder RenderGraph::addPass(std::string_view name)
	{

		uint32_t index = m_passes.size();
		m_passes.emplace_back(std::make_unique<RenderGraphPass>(name, this));

		RenderGraphPass* pass = m_passes.back().get();

		auto it = m_pass_to_idx_map.find(pass->name());
		if (it != m_pass_to_idx_map.end())
		{
			CRITICAL_ERROR_LOG("Cannot create two identically named rendergraph passes")
		}
		else
		{
			m_pass_to_idx_map.emplace(pass->name(), index);
			pass->m_pass_index = index;
		}


		return RenderGraphPassBuilder(pass);

	}


	std::pair<bool, RenderGraphBufferResource*> RenderGraph::addBufferResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return {true, static_cast<RenderGraphBufferResource*>(m_resources[index].get())};
		}
		else
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<RenderGraphBufferResource>(RenderGraphBufferResource(index)));
			
			return {false, static_cast<RenderGraphBufferResource*>(resource.get())};
		}
	}
	




	std::pair<bool, RenderGraphImageResource*> RenderGraph::addImageResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end()) //found
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return {true, static_cast<RenderGraphImageResource*>(m_resources[index].get())};
		}
		else //added
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<RenderGraphImageResource>(RenderGraphImageResource(index)));

			return {false, static_cast<RenderGraphImageResource*>(resource.get())};
		}
	}




	bool RenderGraph::setBackbuffer(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;
			m_backbuffer_idx = index;

			return true;
		}
		else
		{
			return false;
		}
	}


	RenderGraphResource* RenderGraph::getResource(uint32_t index)
	{
		return m_resources[index].get(); 
	}



	void RenderGraph::execute(CommandBuffer& cmd)
	{

		//Validate graph (Sanity check) and remove pass write count of unread resources
		if (!validateGraph())
		{
			CRITICAL_ERROR_LOG("RenderGraph not valid");
		}

		//Setup graph passes order and cull unused resources
		m_next_resources.push(m_backbuffer_idx);
		setupGraphPassOrder(m_next_passes, m_next_resources);


		//Allocate physical resources 
		allocatePhysicalResources();


		//Build descriptors and framebuffers/renderpasses  
		/*TODO: Currently renderpass / framebuffer pair is created for each rendergraphpass.
		* Possible performance improvements: hash renderpass and framebuffer objects and reuse them if rendergraph doesnt change with respect to previous frames
		* If the renderpass/framebuffer is not used after a couple of frames (8 or so) then it could be deleted		
 		*/
		for (auto pass_idx : m_pass_stack)
		{
			m_passes[pass_idx]->buildFramebuffer();
			m_passes[pass_idx]->buildDescriptors();
		}



		//TODO NEXT barriers. 



		//TODO resource aliasing
		//	FROM themaister.net: The algorithm is fairly straight forward. For each resource we figure out the first and last physical render pass where a resource is used. 																								
		//	If we find another resource with the same dimensions/format, and their pass range does not overlap, presto, we can alias! 
		//	We inject some information where we can transition “ownership” between resources.


		//Execute rendergraph passes
		for (auto pass_idx : m_pass_stack)
		{
			m_passes[pass_idx]->executePass(cmd);
			
		}




	}



	//TODO print to conosole reason why it didnt validate
	bool RenderGraph::validateGraph()
	{

		//check output buffer
		if (m_backbuffer_idx < 0)
		{
			return false;
		}

		
		for (auto& pass_ptr : m_passes)
		{
			auto& pass = *pass_ptr;

			//TODO: check that input and outputs match in dimensions etc etc

		}


		for (auto& resource_ptr : m_resources)
		{
			auto& resource = *resource_ptr;

			//If resource is not read, reduce resource write count from pass that wrote resource (except for backbuffer)
			if (resource.readInPasses().size() == 0) //TODO if resource is not read after last write?
			{
				if (resource.index() == m_backbuffer_idx) continue; //Do not reduce resource write for backbuffer

				for (auto& pass_idx : resource.writtenInPasses())
				{
					m_passes[pass_idx].get()->m_resource_write_count--;

					if (m_passes[pass_idx].get()->m_resource_write_count < 0)
					{
						CRITICAL_ERROR_LOG("RenderGraphPass resource write count cannot be smaller than 0");
					}
				}
			}
		}
		return true;

	}




	void RenderGraph::setupGraphPassOrder(std::stack<uint32_t>& next_passes, std::stack<uint32_t>& next_resources)
	{
		while(!next_passes.empty() || !next_resources.empty())
		{ 
			//processResourceDependencies;
			while (!next_resources.empty())
			{
				auto res_idx = next_resources.top();
				auto& resource = *m_resources[res_idx];
			
				//For each pass that writes to current resource, reduce write ref count. If refcount is 0 add that pass to next passes stack to be processed
				for (auto& pass_idx : resource.writtenInPasses())
				{
					m_passes[pass_idx]->m_resource_write_count--;
					if (m_passes[pass_idx]->m_resource_write_count <= 0) //TODO its never <= to 0 with "aliasing resource names"
					{
						next_passes.push(pass_idx);
						m_pass_stack.emplace_back(pass_idx);//add pass to final order of passses
					}
				}
				next_resources.pop();
			}



			//processPassDependencies;
			while (!next_passes.empty())
			{
				auto pass_idx = next_passes.top();
				auto& pass = *m_passes[pass_idx];

				//For each resource that is read by current pass, reduce read ref count of that resource. If refcount is 0 add that resource to next resource to be processed
				for (auto& res : pass.m_resource_reads)
				{
					res->passReadsRefCount()--;
					if (res->passReadsRefCount() <= 0)
					{
						next_resources.push(res->index());

						//validate resource
						res->setUsedInGraph(true);
					}
				}

				//For each resource written by current pass, validate that resource as a resource that is used in the rendergraph and will need a "physical" 
				for (auto& res : pass.m_resource_writes)
				{
					res->setUsedInGraph(true);
				}
				next_passes.pop();
			}
		}

		std::reverse(m_pass_stack.begin(), m_pass_stack.end());
	}
	





	void RenderGraph::allocatePhysicalResources()
	{
		//Must do aliasing of read modify write resources. dont assign different physical resources

		for (auto& resource : m_resources)
		{
			if(!resource->usedInGraph()) continue;

			if (resource->physicalIndex() == RenderGraphResource::Unset)
			{

				if (resource->resourceType() == RenderGraphResource::ResourceType::Image)
				{
					m_resource_pool->createImage(static_cast<RenderGraphImageResource*>(resource.get()));
				}
				else if (resource->resourceType() == RenderGraphResource::ResourceType::Buffer)
				{
					m_resource_pool->createBuffer(static_cast<RenderGraphBufferResource*>(resource.get()));
				}
			}

		}
	}



	RenderGraph::~RenderGraph()
	{
		m_resource_pool->recycleRenderGraph();
	}

}