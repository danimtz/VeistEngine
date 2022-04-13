#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"

namespace Veist
{


	struct DeferredRenderer
	{

		//TODO this should be in editorPass or editor renderer not here
		RenderGraphImageResource* m_editor_target{nullptr};


		static DeferredRenderer createRenderer(RenderGraph& render_graph, ecs::EntityRegistry* scene_registry);






	};


}

