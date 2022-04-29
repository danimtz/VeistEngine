#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"

namespace Veist
{


	struct BasicRenderer
	{

		glm::vec2 m_size = {};
		RenderGraph::ImageResource* m_renderer_target{ nullptr };

		static BasicRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size);



	};


}

