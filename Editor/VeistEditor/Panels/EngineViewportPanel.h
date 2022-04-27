#pragma once

#include <VeistEditor.h>

#include "GUIPanel.h"

#include "Veist/Camera/CameraController.h"
#include "Veist/Events/EditorEvents.h"

#include "Veist/RenderGraph/RenderGraph.h"

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
		void drawMenuBar();

		void renderScene();

		void changeScene(EditorSceneChangedEvent& event);


		std::unique_ptr<ColorTextureAttachment> m_no_scene_img;
		Framebuffer m_no_scene_framebuffer;

		std::shared_ptr<RenderGraph::ResourcePool> m_resource_pool;
		ImTextureID m_texture_id;

		glm::vec2 m_viewport_size = {1920,1080};

		bool m_viewport_focused = true;
		bool m_viewport_hovered = true;

		std::unique_ptr<CameraController> m_editor_camera;
		Scene* m_active_scene = nullptr;


		static float m_aspect_ratio;


	};



}
