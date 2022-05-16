

#include "Veist/Renderer/Renderer.h"
namespace Veist
{


	struct DeferredRenderer : public Renderer
	{

		//glm::vec2 m_size = {};
		//RenderGraph::ImageResource* m_renderer_target{ nullptr };
		uint32_t m_shadow_map_size = 1024;


		static DeferredRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size);


	};


}

