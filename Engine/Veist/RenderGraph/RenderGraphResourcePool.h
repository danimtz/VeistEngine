#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "RenderGraphResource.h"


namespace Veist
{
	

	class RenderGraphResourcePool
	{
	public:
		static constexpr uint32_t cull_after_x_frames = 8;

		RenderGraphResourcePool() = default;

		void createImage(RenderGraphImageResource* resource);
		void createBuffer(RenderGraphBufferResource* resource);

		ImageBase* getImage(RenderGraphImageResource* resource) { return &m_physical_images[resource->physicalIndex()];};
		ShaderBuffer* getBuffer(RenderGraphBufferResource* resource) { return &m_physical_buffers[resource->physicalIndex()]; };

		void recycleRenderGraph();

	private:
	
		bool reuseImageFromPool(RenderGraphImageResource* resource);
		bool reuseBufferFromPool(RenderGraphBufferResource* resource);
		
		std::vector<ImageBase> m_physical_images;
		std::vector<ShaderBuffer> m_physical_buffers;

		std::forward_list<std::pair<uint32_t, ShaderBuffer>> m_unused_buffer_pool;
		std::forward_list<std::pair<uint32_t, ImageBase>> m_unused_image_pool;


	};


}

