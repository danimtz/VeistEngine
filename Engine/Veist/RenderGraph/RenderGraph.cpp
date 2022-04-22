#include "pch.h"
#include "RenderGraph.h"

//#include "PassBuilder.h"


namespace Veist
{
namespace RenderGraph
{


	PassBuilder RenderGraph::addPass(std::string_view name)
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


		return PassBuilder(pass);

	}


	std::pair<bool, BufferResource*> RenderGraph::addBufferResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return {true, static_cast<BufferResource*>(m_resources[index].get())};
		}
		else
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<BufferResource>(BufferResource(index)));
			
			return {false, static_cast<BufferResource*>(resource.get())};
		}
	}
	




	std::pair<bool, ImageResource*> RenderGraph::addImageResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end()) //found
		{
			//todo? assert if its the correct type of resource
			uint32_t index = it->second;

			return {true, static_cast<ImageResource*>(m_resources[index].get())};
		}
		else //added
		{
			uint32_t index = m_resources.size();

			m_resource_to_idx_map.emplace(name, index);

			auto& resource = m_resources.emplace_back(std::make_unique<ImageResource>(ImageResource(index)));

			return {false, static_cast<ImageResource*>(resource.get())};
		}
	}



	BufferResource* RenderGraph::getBufferResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end())
		{
			uint32_t index = it->second;

			return static_cast<BufferResource*>(m_resources[index].get()) ;
		}
		else
		{
			CRITICAL_ERROR_LOG("Buffer Resource was not found");
		}
	}



	ImageResource* RenderGraph::getImageResource(const std::string& name)
	{
		auto it = m_resource_to_idx_map.find(name);
		if (it != m_resource_to_idx_map.end()) //found
		{
			uint32_t index = it->second;

			return  static_cast<ImageResource*>(m_resources[index].get()) ;
		}
		else 
		{
			CRITICAL_ERROR_LOG("Image Resource was not found");
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



	Resource* RenderGraph::getResource(uint32_t index)
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

		//Setup physical resources info
		setupPhysicalResources();

		aliasResources();

		//Allocate the actual images/buffers
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

		

		//Execute rendergraph passes
		for (auto pass_idx : m_pass_stack)
		{
			//build and execute barriers;
			createBarriers(pass_idx, cmd);


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
	




	void RenderGraph::setupPhysicalResources()
	{

		//TODO: Fill m_physical_resources with aliased versions of resources

		for (auto& resource : m_resources)
		{
			if (!resource->usedInGraph()) continue;

			if (resource->resourceType() == ResourceType::Image)
			{
				Resource* res_ptr = resource.get();
				ImageResource* phys_img = static_cast<ImageResource*>(res_ptr);
				m_physical_resources.emplace_back(std::make_unique<PhysicalImage>(phys_img));
			}
			else if (resource->resourceType() == ResourceType::Buffer)
			{
				Resource* res_ptr = resource.get();
				BufferResource* phys_buff = static_cast<BufferResource*>(res_ptr);
				m_physical_resources.emplace_back(std::make_unique<PhysicalBuffer>(phys_buff));
			}
		}
	}



	void RenderGraph::aliasResources()
	{

		
		//Aliasing of Read Modify Write resources
		for (auto pass_idx : m_pass_stack)
		{
			auto* pass = m_passes[pass_idx].get();

			//Color inputs/outputs
			for (int i = 0; i < pass->m_color_inputs.size(); i++)
			{
				if (pass->m_color_inputs[i] == nullptr || pass->m_color_outputs[i] == nullptr) continue;

				auto* physical_image = static_cast<PhysicalImage*>(pass->m_color_outputs[i]->physicalResource());
				auto* resource_to_alias = pass->m_color_inputs[i];

				physical_image->aliasImageResource(resource_to_alias);

			}

			//Depth input/output

			if (pass->m_depth_input != nullptr && pass->m_depth_output != nullptr)
			{
				auto* physical_image = static_cast<PhysicalImage*>(pass->m_depth_output->physicalResource());
				auto* resource_to_alias = pass->m_depth_input;

				physical_image->aliasImageResource(resource_to_alias);
			}

		
			//Rest
			//TODO buffer/storage/etc rmw aliasing

		}
		
		
		//TODO resource aliasing
		//	FROM themaister.net: The algorithm is fairly straight forward. For each resource we figure out the first and last physical render pass where a resource is used. 																								
		//	If we find another resource with the same dimensions/format, and their pass range does not overlap, presto, we can alias! 
		//	We inject some information where we can transition “ownership” between resources.



		//Update aliased resource pointers in each rendergraphpass
		/*for (auto pass_idx : m_pass_stack)
		{
			auto* pass = m_passes[pass_idx].get();

			//Add resources to pass resources for barrier 
			for (auto res : pass->m_pass_resources)
			{
				if (res->usedInGraph())
				{
					pass->m_pass_physical_resources.emplace(res);
				}
			}

			//Fix color/depth inputs/outputs for framebuffer creation
			for (int i = 0; i < pass->m_color_outputs.size(); i++)
			{
				if (pass->m_color_inputs[i] != nullptr && !pass->m_color_inputs[i]->usedInGraph())
				{
					auto& name = pass->m_color_inputs[i]->name();
					pass->m_color_inputs[i] = static_cast<ImageResource*>(m_resources[m_resource_to_idx_map[name]].get());
				}

				if (!pass->m_color_outputs[i]->usedInGraph())
				{
					auto& name = pass->m_color_outputs[i]->name();
					pass->m_color_outputs[i] = static_cast<ImageResource*>(m_resources[m_resource_to_idx_map[name]].get());
				}
			}
				
			if (pass->m_depth_input != nullptr && pass->m_depth_input->usedInGraph())
			{
				auto& name = pass->m_depth_input->name();
				pass->m_depth_input = static_cast<ImageResource*>(m_resources[m_resource_to_idx_map[name]].get());
			}

			if (pass->m_depth_output != nullptr && pass->m_depth_output->usedInGraph())
			{
				auto& name = pass->m_depth_output->name();
				pass->m_depth_output = static_cast<ImageResource*>(m_resources[m_resource_to_idx_map[name]].get());
			}

		}*/
	}



	void RenderGraph::allocatePhysicalResources()
	{

		//Assign an Image/Buffer to the set of physical resources

		for (auto& it = m_physical_resources.begin(); it != m_physical_resources.end(); )
		{
			auto resource = it->get();
			if (resource->isUnused())
			{
				//If physical resource unused due to being aliased away remove it
				it = m_physical_resources.erase(it);
			}
			else
			{
				if (resource->resourceType() == ResourceType::Image)
				{
					m_resource_pool->createImage(static_cast<PhysicalImage*>(resource));
				}
				else if (resource->resourceType() == ResourceType::Buffer)
				{
					m_resource_pool->createBuffer(static_cast<PhysicalBuffer*>(resource));
				}

				//Add physical resource to passes that use it
				for (auto pass_idx : resource->usedInPasses())
				{
					m_passes[pass_idx]->m_pass_physical_resources.emplace(resource);
				}
				it++;
			}
		}

		/*
		for (auto& resource : m_physical_resources)
		{
			
			if (resource->physicalIndex() == PhysicalResource::Unused)
			{

				if (resource->resourceType() == ResourceType::Image)
				{
					m_resource_pool->createImage(static_cast<PhysicalImage*>(resource.get()));
				}
				else if (resource->resourceType() == ResourceType::Buffer)
				{
					m_resource_pool->createBuffer(static_cast<PhysicalBuffer*>(resource.get()));
				}
			}

		}*/
	}

	

	void RenderGraph::createBarriers(uint32_t pass_idx, CommandBuffer& cmd)
	{
		struct BarrierInfo
		{
			PipelineStage dst_stage;
			//access?
			VkImageLayout new_layout;
		};

		std::vector<ImageBarrier> image_barriers;
		std::vector<BufferBarrier> buffer_barriers;

		for (auto res : m_passes[pass_idx]->m_pass_physical_resources)
		{
			if (res->resourceType() == ResourceType::Image)
			{
				auto img_res = static_cast<PhysicalImage*>(res);
				if (img_res->lastUsedPass() != PhysicalResource::Unused)
				{
					//src as late as possible and dst as early as possible to ensure sync
					ImageBase* image = m_resource_pool->getImage(img_res);
					PipelineStage src_stage = img_res->getStageInPass(img_res->lastUsedPass());
					PipelineStage dst_stage = img_res->getStageInPass(pass_idx);
					VkImageLayout old_layout = getImageLayout(img_res->imageUsageInPass(img_res->lastUsedPass()));
					VkImageLayout new_layout = getImageLayout(img_res->imageUsageInPass(pass_idx));

					image_barriers.emplace_back(image, src_stage, dst_stage, old_layout, new_layout);
				}
				else
				{
					ImageBase* image = m_resource_pool->getImage(img_res);
					PipelineStage src_stage = PipelineStage::TopOfPipe;
					PipelineStage dst_stage = img_res->getStageInPass(pass_idx);
					VkImageLayout old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
					VkImageLayout new_layout = getImageLayout(img_res->imageUsageInPass(pass_idx));

					image_barriers.emplace_back(image, src_stage, dst_stage, old_layout, new_layout);
				}



			}
			else if (res->resourceType() == ResourceType::Buffer)
			{
				auto buff_res = static_cast<PhysicalBuffer*>(res);
				if (buff_res->lastUsedPass() != PhysicalResource::Unused)
				{

					//src as late as possible and dst as early as possible to ensure sync
					ShaderBuffer* buffer = m_resource_pool->getBuffer(buff_res);
					PipelineStage src_stage = buff_res->getStageInPass(buff_res->lastUsedPass());
					PipelineStage dst_stage = buff_res->getStageInPass(pass_idx);
				
					buffer_barriers.emplace_back(buffer, src_stage, dst_stage);
				}
			}

			res->setLastUsedPass(pass_idx);
		}

		cmd.pipelineBarrier(image_barriers, buffer_barriers);

		/*
		//resource reads
		for (auto res : m_passes[pass_idx]->m_resource_reads)
		{
			if (res->resourceType() == Resource::ResourceType::Image)
			{
				auto img_res = static_cast<ImageResource*>(res);
				
				//src as late as possible and dst as early as possible to ensure sync



			}
			else if(res->resourceType() == Resource::ResourceType::Buffer)
			{
				auto buff_res = static_cast<BufferResource*>(res);


			}


		}
		
		//resource writes
		for (auto res : m_passes[pass_idx]->m_resource_writes)
		{

		}*/
	}



	RenderGraph::~RenderGraph()
	{
		m_resource_pool->recycleRenderGraph();
	}

}
}