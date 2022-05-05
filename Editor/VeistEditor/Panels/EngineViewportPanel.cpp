#include <pch.h>
#include "EngineViewportPanel.h"

#include "VeistEditor/EditorApp.h"



//#include "Veist/Renderer/BasicRenderer.h"
//#include "Veist/Renderer/DeferredRenderer.h"
#include "VeistEditor/EditorGraphics/EditorRenderer.h"

//TEMPORARY, replace with renderer alter that uses the rendergraph

#include "Veist/Scenes/ECS/Components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
//#include "Veist/RenderGraph/RenderGraphPassBuilder.h"

namespace VeistEditor
{
	
	float EngineViewportPanel::m_aspect_ratio = 16.0f/9.0f;

	EngineViewportPanel::EngineViewportPanel()
	{
		
	

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
		viewport_panel_size.x = viewport_panel_size.y * m_aspect_ratio;
		m_viewport_size = { viewport_panel_size.x, viewport_panel_size.y };
		ImGui::Image(m_texture_id, viewport_panel_size);
		ImGui::End();
		ImGui::PopStyleVar();


	
	}

	void EngineViewportPanel::renderScene()
	{
		if (!m_active_scene->ecsRegistry()->isSceneLoaded())
		{
			return;
		}

		RenderGraph::RenderGraph render_graph(m_resource_pool);

		//BasicRenderer renderer = BasicRenderer::createRenderer(render_graph, m_active_scene->ecsRegistry());
		//DeferredRenderer renderer = DeferredRenderer::createRenderer(render_graph, m_active_scene->ecsRegistry(), m_viewport_size);
		
		EditorRenderer renderer = EditorRenderer::createRenderer(render_graph, m_active_scene->ecsRegistry(), m_viewport_size);
		
		render_graph.execute(RenderModule::getBackend()->getCurrentCmdBuffer());
	

		//TODO this should be part of an editor pass (and its pretty bad especially the static cast to get the image base)
		/*std::vector<Descriptor> descriptor;
		ImageBase* target = render_graph.resourcePool()->getImage(static_cast<RenderGraph::PhysicalImage*>(renderer.m_editor_target->physicalResource()))->image.get();
		descriptor.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, target, SamplerType::RepeatLinear, VK_SHADER_STAGE_FRAGMENT_BIT);

		DescriptorSet desc_set{ 0, descriptor };*/

		m_texture_id = renderer.getImGuiTextureId();//ImGui_ImplVulkan_AddTexture(sampler->sampler(), final_image->imageView(), getImageLayout(final_image->imageUsage()));


	}


	void EngineViewportPanel::drawMenuBar()
	{

		/*ImGui::BeginMenuBar();
		
		
			ImGui::BeginMenu("Camera");
			
				if (ImGui::MenuItem("Reset camera"))
				{
					//_scene_view.camera().set_view(Camera().view_matrix());
				}
			ImGui::EndMenu();
			

		ImGui::EndMenuBar();
		
		*/

	}



	void EngineViewportPanel::changeScene(EditorSceneChangedEvent& event)
	{
		m_active_scene = event.getNewScene();
		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());

	}
}
