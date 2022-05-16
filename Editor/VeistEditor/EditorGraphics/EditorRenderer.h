
#include "Veist/Renderer/Renderer.h"
namespace Veist
{

	enum class DeferredTarget : uint32_t
	{
		Shaded = 0,
		Albedo = 1,
		Normals = 2,
		Metallic = 3,
		Roughness = 4,
		Depth = 5,
		AO = 6

	};


	class EditorRenderer
	{
	public:
		
		static EditorRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size, uint32_t view_target, uint32_t fill_type, uint32_t renderer_type);

		void* getImGuiTextureId();

		RenderGraph::ImageResource* m_editor_target{ nullptr };
		Renderer m_renderer;
		//BasicRenderer m_renderer;

	private:

		std::unique_ptr<DescriptorSet> m_imgui_texture_id;
		RenderGraph::RenderGraph* m_render_graph;

	};


}

