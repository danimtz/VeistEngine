#pragma once

#include <VeistEditor.h>

#include "GUIPanel.h"

#include "Veist/Camera/CameraController.h"
#include "Veist/Events/EditorEvents.h"

#include "Veist/RenderGraph/RenderGraph.h"

#include "VeistEditor/EditorGraphics/EditorRenderer.h"

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

		enum class ObjectFill : uint32_t
		{
			Shaded = 0,
			Wireframe = 1,
			
		};

		enum class RendererType : uint32_t
		{
			Forward = 0,
			Deferred = 1,

		};

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


		float m_aspect_ratio;


		std::unique_ptr<Veist::RendererSettings> m_renderer_settings;

		DeferredTarget m_view_target;
		ObjectFill m_fill_type;
		RendererType m_renderer_type;
		bool m_deferred_renderer_enabled{true};
	};



}
