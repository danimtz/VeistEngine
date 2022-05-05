#include "pch.h"
#include "Resource.h"


namespace Veist
{
namespace RenderGraph
{

	void Resource::setReadInPass(uint32_t pass_index)
	{
		m_read_in_passes.emplace(pass_index);
		m_read_in_pass_count = m_read_in_passes.size();
	}

	void Resource::addStage(PipelineStage stage, uint32_t pass_index)
	{
		auto& it = m_stage_in_pass.find(pass_index);
		if (it == m_stage_in_pass.end())
		{
			m_stage_in_pass.emplace(pass_index, stage);
		}
		else
		{
			it->second = it->second | stage;
		}
	};


	void ImageResource::setImageInfo(const ImageInfo& info)
	{
		if (m_info.properties.imageSize().width == 0)
		{
			m_info = info;
		}
	}

	void ImageResource::addImageUsage(ImageUsage usage, uint32_t pass_index)
	{
		m_usage = m_usage | usage;

		auto it = m_usage_in_pass.find(pass_index);
		if (it == m_usage_in_pass.end())
		{
			m_usage_in_pass.emplace(pass_index, usage);
		}
		else
		{
			it->second = it->second | usage;
		}
	}


	void PhysicalResource::aliasResource( Resource* resource)
	{
		//Merge stage in passes
		if (resource->m_physical_resource != nullptr)
		{
			auto phys_resource = resource->m_physical_resource;
			for (auto idx_stage_pair : phys_resource->m_stage_in_pass)
			{
				auto index = idx_stage_pair.first;
				auto stage = idx_stage_pair.second;

				auto it = m_stage_in_pass.find(index);
				if (it == m_stage_in_pass.end())
				{
					m_stage_in_pass.emplace(index, stage);
				}
				else
				{
					it->second = it->second | stage;
				}

				m_used_in_passes.emplace(index);
			}

			
			
			//Merge physical resource aliases together
			PhysicalResource* res_ptr = resource->m_physical_resource;
			for (auto& res : resource->m_physical_resource->m_resource_aliases)
			{
				m_resource_aliases.emplace(res);
				res->m_physical_resource = this;
			}
			res_ptr->m_resource_aliases.clear();


			/*if (resource->m_physical_resource != nullptr)//////
			{////
				resource->m_physical_resource->m_resource_aliases.erase(resource);/////
			}/////

			m_resource_aliases.emplace(resource);////*/

		}
		else
		{
			for (auto idx_stage_pair : resource->m_stage_in_pass)
			{
				auto index = idx_stage_pair.first;
				auto stage = idx_stage_pair.second;

				auto it = m_stage_in_pass.find(index);
				if (it == m_stage_in_pass.end())
				{
					m_stage_in_pass.emplace(index, stage);
				}
				else
				{
					it->second = it->second | stage;
				}

				m_used_in_passes.emplace(index);
			}

			m_resource_aliases.emplace(resource);
		}
		

		

		//m_resource_aliases.emplace(resource);////


		resource->m_physical_resource = this;
	}



	PipelineStage PhysicalResource::getStageInPass(uint32_t pass_index) const
	{
		auto it = m_stage_in_pass.find(pass_index);
		if (it == m_stage_in_pass.end())
		{
			//If no stage return top of pipe
			return PipelineStage::TopOfPipe;
		}
		else
		{
			return it->second;
		}
	};



	PhysicalImage::PhysicalImage(ImageResource* resource) : PhysicalResource(ResourceType::Image), m_usage(ImageUsage::None)
	{
		m_info = resource->m_info;
		aliasImageResource(resource);
	}


	void PhysicalImage::aliasImageResource( ImageResource* resource)
	{
		//TODO check that aliased resource can be aliased (compare info)

		if (resource->m_physical_resource != nullptr)
		{
			auto phys_resource = static_cast<PhysicalImage*>(resource->m_physical_resource);
			//Merge usage in passes
			m_usage = m_usage | phys_resource->m_usage;

			//Merge image usage
			for (auto idx_usage_pair : phys_resource->m_usage_in_pass)
			{
				auto index = idx_usage_pair.first;
				auto usage = idx_usage_pair.second;
				auto it = m_usage_in_pass.find(index);
				if (it == m_usage_in_pass.end())
				{
					m_usage_in_pass.emplace(index, usage);
				}
				else
				{
					it->second = it->second | usage;
				}
			}
		}
		else
		{
			//Merge usage in passes
			m_usage = m_usage | resource->m_usage;

			//Merge image usage
			for (auto idx_usage_pair : resource->m_usage_in_pass)
			{
				auto index = idx_usage_pair.first;
				auto usage = idx_usage_pair.second;
				auto it = m_usage_in_pass.find(index);
				if (it == m_usage_in_pass.end())
				{
					m_usage_in_pass.emplace(index, usage);
				}
				else
				{
					it->second = it->second | usage;
				}
			}
		}
		
		aliasResource(resource);
	}



	ImageUsage PhysicalImage::imageUsageInPass(uint32_t pass_index) const
	{
		auto it = m_usage_in_pass.find(pass_index);
		if (it == m_usage_in_pass.end())
		{
			CRITICAL_ERROR_LOG("Resource not used in pass index queried");
		}
		else
		{
			return it->second;
		}
	};

	

	PipelineStage PhysicalResource::getLastStageInFrame(const std::vector<uint32_t>& pass_stack)
	{
		for (auto idx_it = pass_stack.rbegin(); idx_it != pass_stack.rend(); idx_it++)
		{
			uint32_t idx = *idx_it;
			auto it = m_stage_in_pass.find(*idx_it);
			if (it == m_stage_in_pass.end())
			{
				continue;
			}
			else
			{
				return it->second;
			}
		}
		
		return PipelineStage::TopOfPipe;

	}




	PhysicalBuffer::PhysicalBuffer(BufferResource* resource) : PhysicalResource(ResourceType::Buffer)
	{
		m_info = resource->m_info;
		m_usage = resource->m_usage;
		aliasBufferResource(resource);
	}





	void PhysicalBuffer::aliasBufferResource(BufferResource* resource)
	{
		//TODO check that aliased resource can be aliased (compare info)

		aliasResource(resource);
	}


}
}