#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "Resource.h"


namespace Veist
{
namespace RenderGraph
{



	class ResourcePool
	{
	public:
		static constexpr uint32_t cull_after_x_frames = 8;

		ResourcePool() = default;

		void createImage(PhysicalImage* resource);
		void createBuffer(PhysicalBuffer* resource);

		ImageBase* getImage(PhysicalImage* resource) { return m_physical_images[resource->physicalIndex()].get();};
		ShaderBuffer* getBuffer(PhysicalBuffer* resource) { return m_physical_buffers[resource->physicalIndex()].get(); };

		void recycleRenderGraph();

	private:
	
		
		bool reuseImageFromPool(PhysicalImage* resource);
		bool reuseBufferFromPool(PhysicalBuffer* resource);
		

		std::vector<std::unique_ptr<ImageBase>> m_physical_images;
		std::vector<std::unique_ptr<ShaderBuffer>> m_physical_buffers;


		//Store images and buffers that might still be in use here
		std::list<std::pair<VkFence, std::unique_ptr<ImageBase>>> m_in_use_images;
		std::list<std::pair<VkFence, std::unique_ptr<ShaderBuffer>>> m_in_use_buffers;


		std::list<std::pair<uint32_t, std::unique_ptr<ImageBase>>> m_unused_image_pool;
		std::list<std::pair<uint32_t, std::unique_ptr<ShaderBuffer>>> m_unused_buffer_pool;


	};

}
}

