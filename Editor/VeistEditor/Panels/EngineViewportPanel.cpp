#include <pch.h>
#include "EngineViewportPanel.h"

#include "VeistEditor/EditorApp.h"


namespace VeistEditor
{
	
	float EngineViewportPanel::m_aspect_ratio = 16.0f/9.0f;

	EngineViewportPanel::EngineViewportPanel()
	{
		//TODO: image props should be the size of the imgui window
		ImageProperties img_props = ImageProperties({ m_viewport_size }, {VK_FORMAT_R8G8B8A8_SRGB});
		ImageProperties depth_img_props = ImageProperties({ m_viewport_size }, { VK_FORMAT_D32_SFLOAT });

		
		m_framebuffer_image = std::make_unique<ColorTextureAttachment>(img_props);
		m_depth_image = std::make_unique<DepthTextureAttachment>(depth_img_props);


		//Create framebuffer (only using 1 image for now)
		m_target_framebuffer = { m_framebuffer_image.get(), m_depth_image.get(), Framebuffer::LoadOp::Clear };


		m_texture_id = ImGui_ImplVulkan_AddTexture(Sampler(SamplerType::RepeatLinear).sampler(), m_framebuffer_image.get()->imageView(), getImageLayout(m_framebuffer_image.get()->imageUsage()));
	
		
		m_active_scene = EditorApp::get().getActiveScene();

		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());
	
	}

	EngineViewportPanel::~EngineViewportPanel()
	{

	}


	void EngineViewportPanel::onEvent(Event& event)
	{
		EventHandler handler(event);

		handler.handleEvent<EditorSceneChangedEvent>(VEIST_EVENT_BIND_FUNCTION(EngineViewportPanel::changeScene));
		
		if (m_viewport_focused)
		{
			m_editor_camera->onEvent(event);
		}
	}


	void EngineViewportPanel::onDrawPanel()
	{

		update();

		renderPanel();

	}


	void EngineViewportPanel::update()
	{

		if(m_viewport_focused)
		{
			m_editor_camera->onUpdate(EditorApp::get().getFrametime());
		}

	}


	void EngineViewportPanel::renderPanel()
	{
	
		struct PanelConstraint
		{
			static void Square(ImGuiSizeCallbackData* data)
			{
				data->DesiredSize.x = data->DesiredSize.y * m_aspect_ratio;
				//data->DesiredSize.y = data->DesiredSize.x * (1.0f/m_aspect_ratio);
			}
		};
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), PanelConstraint::Square);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr);


		renderScene();


		//Check viewport is focused
		if (m_viewport_hovered && !m_viewport_focused && ImGui::IsMouseDown(1))
		{
			ImGui::SetWindowFocus();
		}
		m_viewport_focused = ImGui::IsWindowFocused();
		m_viewport_hovered = ImGui::IsWindowHovered();
		

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		viewport_panel_size.x = viewport_panel_size.y * m_aspect_ratio;
		m_viewport_size = { viewport_panel_size.x, viewport_panel_size.y };
		ImGui::Image(m_texture_id, viewport_panel_size);
		ImGui::End();
		ImGui::PopStyleVar();
	
	}

	void EngineViewportPanel::renderScene()
	{

		CommandBuffer& cmd_buffer = RenderModule::getBackend()->getCurrentCmdBuffer();
		cmd_buffer.beginRenderPass(m_target_framebuffer);
		RenderModule::renderScene(cmd_buffer);
		cmd_buffer.endRenderPass();

	}




	void EngineViewportPanel::changeScene(EditorSceneChangedEvent& event)
	{
		m_active_scene = event.getNewScene();
		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());

	}
}
