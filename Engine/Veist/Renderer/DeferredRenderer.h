

#include "Veist/Renderer/Renderer.h"
namespace Veist
{


	struct DeferredRenderer : public Renderer
	{



		static DeferredRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size, RendererSettings* settings);




	};


}

