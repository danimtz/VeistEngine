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

		//Remove physical resource pointer from resource if it had one (and from alias list in the old physical resource)
		if(resource->m_physical_resource != nullptr)
		{
			resource->m_physical_resource->m_resource_aliases.erase(resource);
		}

		m_resource_aliases.emplace(resource);
		resource->m_physical_resource = this;
	}



	PipelineStage PhysicalResource::getStageInPass(uint32_t pass_index) const
	{
		auto it = m_stage_in_pass.find(pass_index);
		if (it == m_stage_in_pass.end())
		{
			CRITICAL_ERROR_LOG("Resource does not have a stage assigned for this pass");
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