#pragma once

#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"

namespace Veist
{

	struct Renderer 
	{

		Renderer() = default;
		virtual ~Renderer(){};


		glm::vec2 m_size = {};
		RenderGraph::ImageResource* m_renderer_target{ nullptr };


	};


}