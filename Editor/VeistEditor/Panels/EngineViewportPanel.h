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
		std::unique_ptr<ColorTextureAttachment> m_framebuffer_image;
		std::unique_ptr<DepthTextureAttachment> m_depth_image;
		Framebuffer m_target_framebuffer;
		ImTextureID m_texture_id;

		glm::vec2 m_viewport_size = {1920,1080};

		float m_aspect_ratio = 16.0f/9.0f;


	};



}
