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

		struct ImageData
		{
			ImageData(ImageProperties properties, ImageUsage usage, ImageViewType type, PipelineStage stage) : image(std::make_unique<ImageBase>(properties, usage, type)), last_stage(stage) {}
			
			std::unique_ptr<ImageBase> image;
			PipelineStage last_stage;
		};

		static constexpr uint32_t cull_after_x_frames = 8;

		ResourcePool() = default;

		void createImage(PhysicalImage* resource, PipelineStage stage);
		void createBuffer(PhysicalBuffer* resource);

		ImageData* getImage(PhysicalImage* resource) { return m_physical_images[resource->physicalIndex()].get();};
		ShaderBuffer* getBuffer(PhysicalBuffer* resource) { return m_physical_buffers[resource->physicalIndex()].get(); };

		//TODO: fix that when GPU is slow, fences can loop around and end up with images from 3 fences ago taht have the same fence as the curent fence but it is not in use
		void recycleRenderGraph();

	private:
	
		
		
		bool reuseImageFromPool(PhysicalImage* resource);
		bool reuseBufferFromPool(PhysicalBuffer* resource);
		

		std::vector<std::unique_ptr<ImageData>> m_physical_images;
		std::vector<std::unique_ptr<ShaderBuffer>> m_physical_buffers;


		//Store images and buffers that might still be in use here
		//std::list<std::pair<VkFence, std::unique_ptr<ImageBase>>> m_in_use_images;
		std::list<std::pair<uint32_t, std::unique_ptr<ShaderBuffer>>> m_in_use_buffers;


		std::list<std::pair<uint32_t, std::unique_ptr<ImageData>>> m_unused_image_pool;
		std::list<std::pair<uint32_t, std::unique_ptr<ShaderBuffer>>> m_unused_buffer_pool;


	};

}
}

