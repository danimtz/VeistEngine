#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"

namespace Veist
{


	struct DeferredRenderer
	{

		//TODO this should be in editorPass or editor renderer not here
		RenderGraph::ImageResource* m_editor_target{nullptr};


		static DeferredRenderer createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry);






	};


}

