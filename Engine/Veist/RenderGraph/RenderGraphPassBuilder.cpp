#include "pch.h"
#include "RenderGraphPassBuilder.h"



namespace Veist
{




	void RenderGraphPassBuilder::setRenderFunction(RenderFunction&& function)
	{
		m_graph_pass->m_render_function = std::move(function);
	}





}