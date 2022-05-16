#include "Veist/Renderer/Renderer.h"

namespace Veist
{


	struct WireframeRenderer : public Renderer
	{

		

		static WireframeRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size);



	};


}

