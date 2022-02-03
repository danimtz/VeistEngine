#pragma once

#include <VeistEditor.h>

#include "GUIPanel.h"


namespace VeistEditor
{


	class EngineViewportPanel : public GUIPanel
	{
	public:
		EngineViewportPanel();
		~EngineViewportPanel();

		void renderPanel() override;

	private:
	
		//framebuffer image resources TODO: when i create a framegraph system have these be part of framegraph resources
		std::vector<ColorAttachment> m_framebuffer_images;
		DepthAttachment m_depth_image;
		Framebuffer m_target_framebuffer;



	};



}
