#pragma once


#include <Veist/Graphics/RenderModule.h>
#include "RenderGraphResource.h"


namespace Veist
{
	

	class RenderGraphResourcePool
	{
	public:

		RenderGraphResourcePool() = default;




	private:

		std::vector<ShaderBuffer> m_buffers;
		std::vector<TransientImage<>> m_images;
		std::vector<DescriptorSet> m_descriptor_sets;

	};


}

