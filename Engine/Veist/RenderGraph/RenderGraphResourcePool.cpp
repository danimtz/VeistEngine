#include <pch.h>

#include "RenderGraphResourcePool.h"





namespace Veist
{


	void RenderGraphResourcePool::createImage(RenderGraphImageResource* resource)
	{

		if (!reuseImageFromPool(resource))
		{
			uint32_t index = m_physical_images.size();
			m_physical_images.emplace_back(std::make_unique<ImageBase>(resource->imageProperties(), resource->imageUsage(), resource->imageViewType()));
			resource->setPhysicalIndex(index);

		}
	}




	void RenderGraphResourcePool::createBuffer(RenderGraphBufferResource* resource)
	{
		if (!reuseBufferFromPool(resource))
		{

			uint32_t index = m_physical_buffers.size();
			m_physical_buffers.emplace_back(std::make_unique<ShaderBuffer>(resource->bufferSize(), resource->subBufferCount(), resource->bufferUsage()));
			resource->setPhysicalIndex(index);

		}
	}



	bool RenderGraphResourcePool::reuseImageFromPool(RenderGraphImageResource* resource)
	{
		
		for (auto it = m_unused_image_pool.begin(); it != m_unused_image_pool.end(); it++)
		{
			auto& image = it->second;
			if ((image->properties() == resource->imageProperties()) && (image->imageUsage() == resource->imageUsage()))
			{
				uint32_t index = m_physical_images.size();
				m_physical_images.emplace_back(std::move(image)); //move contruct image to used images vector
				resource->setPhysicalIndex(index);

				//Remove image from pool
				m_unused_image_pool.erase(it);
				return true;
			}

				
		}
		return false;
	}



	bool RenderGraphResourcePool::reuseBufferFromPool(RenderGraphBufferResource* resource)
	{

		for (auto it = m_unused_buffer_pool.begin(); it != m_unused_buffer_pool.end(); it++)
		{
			auto& buffer = it->second;
			if ((buffer->range() == resource->bufferSize()) && (buffer->subBufferCount() == resource->subBufferCount()) && (buffer->usage() == resource->bufferUsage()))
			{
				uint32_t index = m_physical_buffers.size();
				m_physical_buffers.emplace_back(std::move(buffer)); //move contruct image to used images vector
				resource->setPhysicalIndex(index);

				//Remove image from pool
				m_unused_buffer_pool.erase(it);
				return true;
			}


		}
		return false;
	}



	void RenderGraphResourcePool::recycleRenderGraph()
	{
		//Move unused resources from vector to pool
		VkDevice device = RenderModule::getBackend()->getDevice();
		VkFence current_fence = RenderModule::getBackend()->getCurrentCmdBuffer().fence();
		for (auto& image : m_physical_images)
		{
			m_in_use_images.emplace_front(current_fence, std::move(image));
		}
		m_physical_images.clear();

		for (auto& buffer : m_physical_buffers)
		{
			m_in_use_buffers.emplace_front(current_fence, std::move(buffer));
		}
		m_physical_buffers.clear();

		

		//Once command buffer is finished move buffers and images to unused pool
		for (auto& it = m_in_use_images.begin(); it != m_in_use_images.end(); )
		{
			if (vkGetFenceStatus(device,it->first) == VK_SUCCESS)
			{
				m_unused_image_pool.emplace_front(0, std::move(it->second));
				it = m_in_use_images.erase(it);
			}
			else
			{
				it++;
			}
		}

		for (auto& it = m_in_use_buffers.begin(); it != m_in_use_buffers.end(); )
		{
			if (vkGetFenceStatus(device, it->first) == VK_SUCCESS)
			{
				m_unused_buffer_pool.emplace_front(0, std::move(it->second));
				it = m_in_use_buffers.erase(it);
			}
			else
			{
				it++;
			}
		}



		//Cull resources not used for 8 frames
		for (auto& it = m_unused_image_pool.begin(); it != m_unused_image_pool.end(); )
		{
			if (it->first++ > cull_after_x_frames)
			{
				it = m_unused_image_pool.erase(it);
			}
			else
			{
				it++;
			}
		}

		for (auto& it = m_unused_buffer_pool.begin(); it != m_unused_buffer_pool.end();)
		{
			if (it->first++ > cull_after_x_frames)
			{
				it = m_unused_buffer_pool.erase(it);
			}
			else
			{
				it++;
			}
		}
	}


}
