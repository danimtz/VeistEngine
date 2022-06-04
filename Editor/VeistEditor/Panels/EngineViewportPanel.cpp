#include <pch.h>
#include "EngineViewportPanel.h"

#include "VeistEditor/EditorApp.h"



//#include "Veist/Renderer/BasicRenderer.h"
//#include "Veist/Renderer/DeferredRenderer.h"

//TEMPORARY, replace with renderer alter that uses the rendergraph

#include "Veist/Scenes/ECS/Components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
//#include "Veist/RenderGraph/RenderGraphPassBuilder.h"

namespace VeistEditor
{
	
	

	EngineViewportPanel::EngineViewportPanel() : m_view_target(DeferredTarget::Shaded), m_fill_type(ObjectFill::Shaded), m_renderer_type(RendererType::Deferred)
	{
		
	
		m_renderer_settings = std::make_unique<Veist::RendererSettings>();
		m_resource_pool = std::make_shared<RenderGraph::ResourcePool>();

		
	


		//Create descriptor layout for ImGUI::Image descriptor TODO: move this someplace different
		std::vector<VkDescriptorSetLayoutBinding> binding_array;
		VkDescriptorSetLayoutBinding set_binding = {};
		set_binding.binding = 0;
		set_binding.descriptorCount = 1;
		set_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		set_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding_array.emplace_back(set_binding);
		RenderModule::getBackend()->getDescriptorAllocator()->addDescriptorPool(binding_array);
		


		//TODO: image props should be the size of the imgui window
		//TODO change how the no scene image image is setup
		m_no_scene_img = std::make_unique<ColorTextureAttachment>(ImageProperties({ m_viewport_size }, { VK_FORMAT_R8G8B8A8_SRGB }));
		m_no_scene_img->transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		std::vector<Framebuffer::Attachment> colors = {{m_no_scene_img.get()}};
		m_no_scene_framebuffer = Framebuffer(colors);
		
		m_texture_id = ImGui_ImplVulkan_AddTexture(Sampler(SamplerType::RepeatLinear).sampler(), m_no_scene_img.get()->imageView(), getImageLayout(m_no_scene_img.get()->imageUsage()));
		
		m_active_scene = EditorApp::get().getActiveScene();

		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());
	
		m_aspect_ratio = 16.0f/9.0f;
		
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
	
		
		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
		ImGui::Begin("Viewport", nullptr, flags);


		drawMenuBar();



		renderScene();


		//Check viewport is focused
		if (m_viewport_hovered && !m_viewport_focused && ImGui::IsMouseDown(1))
		{
			ImGui::SetWindowFocus();
		}
		m_viewport_focused = ImGui::IsWindowFocused();
		m_viewport_hovered = ImGui::IsWindowHovered();
		

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		m_viewport_size = { viewport_panel_size.x, viewport_panel_size.y };
		

		//update camera aspect ratio if needed
		
		float old_aspect_ratio = m_aspect_ratio;

		m_aspect_ratio = viewport_panel_size.x/viewport_panel_size.y;
	
		
		if (old_aspect_ratio != m_aspect_ratio)
		{
			m_editor_camera->updateAspectRatio(m_viewport_size.x, m_viewport_size.y);
		}
		

		ImGui::Image(m_texture_id, viewport_panel_size);

		ImGui::End();


	
	}

	void EngineViewportPanel::renderScene()
	{
		if (!m_active_scene->ecsRegistry()->isSceneLoaded())
		{
			return;
		}

		RenderGraph::RenderGraph render_graph(m_resource_pool);

		EditorRenderer renderer = EditorRenderer::createRenderer(render_graph, m_active_scene->ecsRegistry(), m_viewport_size, m_renderer_settings.get(), uint32_t(m_view_target), uint32_t(m_fill_type), uint32_t(m_renderer_type));
		
		render_graph.execute(RenderModule::getBackend()->getCurrentCmdBuffer());
	
		m_texture_id = renderer.getImGuiTextureId();//ImGui_ImplVulkan_AddTexture(sampler->sampler(), final_image->imageView(), getImageLayout(final_image->imageUsage()));


	}


	void EngineViewportPanel::drawMenuBar()
	{

		if(ImGui::BeginMenuBar())
		{ 
			
			if (ImGui::BeginMenu("Render"))
			{
				ImGui::MenuItem("Deferred targets", nullptr, false, false);
				ImGui::Separator();
				{
					const char* target_names[] = {
							"Shaded", "Albedo", "Normals", "Metallic", "Roughness", "Depth", "AO"
					};
					for ( uint32_t i = 0; i < 7; i++)
					{
						bool selected = uint32_t(m_view_target) == i;
						ImGui::MenuItem(target_names[i], nullptr, &selected, m_deferred_renderer_enabled);
						if (selected)
						{
							m_view_target = DeferredTarget(i);
						}
					}
				}

				ImGui::Separator();

				//ImGui::PushFont(ImGui::GetFont());
				ImGui::MenuItem("Shading type", nullptr, false, false);
				//ImGui::PopFont();
				ImGui::Separator();
				{
					const char* shading_type[] = {
							"Shaded", "Wireframe"
					};
					for (int i = 0; i < 2; i++)
					{
						bool selected = uint32_t(m_fill_type) == i;
						ImGui::MenuItem(shading_type[i], nullptr, &selected);
						if (selected)
						{
							m_fill_type = ObjectFill(i);
						}
					}
				}


				ImGui::Separator();

				ImGui::MenuItem("Renderer type", nullptr, false, false);
				//ImGui::PopFont();
				ImGui::Separator();
				{
					const char* render_type[] = {
							"Forward", "Deferred"
					};
					for (int i = 0; i < 2; i++)
					{
						bool selected = uint32_t(m_renderer_type) == i;
						ImGui::MenuItem(render_type[i], nullptr, &selected);
						if (selected)
						{
							m_renderer_type = RendererType(i);
						}
					}
				}
				ImGui::EndMenu();
			}



			//ImGui::SetNextWindowSize(ImVec2(200, 0));
			if (ImGui::BeginMenu("Editor camera"))
			{
				if (ImGui::MenuItem("Reset camera"))
				{
					auto camera = m_editor_camera->camera();

					glm::vec3 pos = glm::vec3{ 0.0f, 0.0f, 3.5f };
					m_editor_camera->setPosition(pos);
					camera->setViewMatrix(glm::lookAt(glm::vec3{ 0.0f, 0.0f, 3.5f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
					camera->setPosition(pos);
					camera->setFoVy(55);
				}

				//ImGui::MenuItem("Camera speed");

				//static bool enabled = true;
				//ImGui::MenuItem("Enabled", "", &enabled);
				//ImGui::BeginChild("child", ImVec2(0, 60), true);
				//for (int i = 0; i < 10; i++)
				//	ImGui::Text("Scrolling Text %d", i);
				//ImGui::EndChild();

				//static int n = 0;
				float& cam_speed = m_editor_camera->cameraSpeed();
				static float max_speed = 25.0f;
				ImGui::SliderFloat("Camera speed", &cam_speed, 0.0f, max_speed);
				ImGui::InputFloat("Max speed", &max_speed, 0.1f);

				//ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
				ImGui::EndMenu();
			}




			ImGui::EndMenuBar();
		}

		
		

	}



	void EngineViewportPanel::changeScene(EditorSceneChangedEvent& event)
	{
		m_active_scene = event.getNewScene();
		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());

	}
}
