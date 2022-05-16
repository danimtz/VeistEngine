
#include "Veist/Renderer/Renderer.h"

namespace Veist
{


	struct BasicRenderer : public Renderer
	{

		

		static BasicRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size);



	};


}

