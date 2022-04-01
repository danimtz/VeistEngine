#include <pch.h>

#include "RenderGraphResourcePool.h"





namespace Veist
{


	void RenderGraphResourcePool::createImage(RenderGraphImageResource* resource)
	{

		if (!reuseImageFromPool(resource))
		{
			uint32_t index = m_physical_images.size();
			m_physical_images.emplace_back(resource->imageProperties(), resource->imageUsage(), resource->imageViewType());
			resource->setPhysicalIndex(index);

		}
	}




	void RenderGraphResourcePool::createBuffer(RenderGraphBufferResource* resource)
	{
		if (!reuseBufferFromPool(resource))
		{

			uint32_t index = m_physical_images.size();
			m_physical_buffers.emplace_back(resource->bufferSize(), resource->subBufferCount(), resource->bufferUsage());
			resource->setPhysicalIndex(index);

		}
	}



	bool RenderGraphResourcePool::reuseImageFromPool(RenderGraphImageResource* resource)
	{
		
		for (auto it = m_unused_image_pool.begin(); it != m_unused_image_pool.end(); it++)
		{
			auto& image = it->second;
			if ((image.properties() == resource->imageProperties()) && (image.imageUsage() == resource->imageUsage()))
			{
				uint32_t index = m_physical_images.size();
				m_physical_images.emplace_back(std::move(image)); //move contruct image to used images vector
				resource->setPhysicalIndex(index);

				//Remove image from pool
				m_unused_image_pool.erase_after(it);
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
			if ((buffer.size() == resource->bufferSize()) && buffer.usage() == resource->bufferUsage())
			{
				uint32_t index = m_physical_buffers.size();
				m_physical_buffers.emplace_back(std::move(buffer)); //move contruct image to used images vector
				resource->setPhysicalIndex(index);

				//Remove image from pool
				m_unused_buffer_pool.erase_after(it);
				return true;
			}


		}
		return false;
	}



	void RenderGraphResourcePool::recycleRenderGraph()
	{
		//Move unused resources from vector to pool
		for (auto& image : m_physical_images)
		{
			m_unused_image_pool.emplace_front(0, std::move(image));
		}
		m_physical_images.clear();

		for (auto& buffer : m_physical_buffers)
		{
			m_unused_buffer_pool.emplace_front(0, std::move(buffer));
		}
		m_physical_buffers.clear();


		//Cull resources not used for 8 frames
		for (auto& it = m_unused_image_pool.begin(); it != m_unused_image_pool.end(); it++)
		{
			if (it->first++ > cull_after_x_frames)
			{
				m_unused_image_pool.erase_after(it);
			}
		}

		for (auto& it = m_unused_buffer_pool.begin(); it != m_unused_buffer_pool.end(); it++)
		{
			if (it->first++ > cull_after_x_frames)
			{
				m_unused_buffer_pool.erase_after(it);
			}
		}
	}


}
