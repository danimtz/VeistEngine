
#include "Veist/Renderer/DeferredRenderer.h"
#include "Veist/Renderer/BasicRenderer.h"
namespace Veist
{


	class EditorRenderer
	{
	public:
		
		static EditorRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size);

		void* getImGuiTextureId();

		RenderGraph::ImageResource* m_editor_target{ nullptr };
		//DeferredRenderer m_renderer;
		BasicRenderer m_renderer;

	private:

		std::unique_ptr<DescriptorSet> m_imgui_texture_id;
		RenderGraph::RenderGraph* m_render_graph;

	};


}

