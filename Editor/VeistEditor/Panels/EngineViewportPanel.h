#pragma once

#include <VeistEditor.h>

#include "GUIPanel.h"

#include "Veist/Camera/CameraController.h"
#include "Veist/Events/EditorEvents.h"

namespace VeistEditor
{


	class EngineViewportPanel : public GUIPanel
	{
	public:
		EngineViewportPanel();
		~EngineViewportPanel();

		void onDrawPanel() override;
		void onEvent(Event& event) override;

	private:
	
		void update();

		void renderPanel();
	
		void renderScene();

		void changeScene(EditorSceneChangedEvent& event);

		//framebuffer image resources TODO: when i create a framegraph system have these be part of framegraph resources
		std::unique_ptr<ColorTextureAttachment> m_framebuffer_image;
		std::unique_ptr<DepthTextureAttachment> m_depth_image;
		Framebuffer m_target_framebuffer;
		ImTextureID m_texture_id;

		glm::vec2 m_viewport_size = {1920,1080};

		bool m_viewport_focused = true;

		std::unique_ptr<CameraController> m_editor_camera;
		Scene* m_active_scene = nullptr;


		static float m_aspect_ratio;


	};



}
