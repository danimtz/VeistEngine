#include <pch.h>
#include "EngineViewportPanel.h"

#include "VeistEditor/EditorApp.h"



#include "Veist/Renderer/BasicRenderer.h"
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
		//TODO: image props should be the size of the imgui window
		ImageProperties img_props = ImageProperties({ m_viewport_size }, {VK_FORMAT_R8G8B8A8_SRGB});
		ImageProperties depth_img_props = ImageProperties({ m_viewport_size }, { VK_FORMAT_D32_SFLOAT });

		m_resource_pool = std::make_shared<RenderGraphResourcePool>();

		m_framebuffer_image = std::make_unique<ColorTextureAttachment>(img_props);
		m_depth_image = std::make_unique<DepthTextureAttachment>(depth_img_props);


		//Create descriptor layout for ImGUI::Image descriptor TODO: move this someplace different
		std::vector<VkDescriptorSetLayoutBinding> binding_array;
		VkDescriptorSetLayoutBinding set_binding = {};
		set_binding.binding = 0;
		set_binding.descriptorCount = 1;
		set_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		set_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding_array.emplace_back(set_binding);
		RenderModule::getBackend()->getDescriptorAllocator()->addDescriptorPool(binding_array);
		
		
		//auto colors = std::vector<Framebuffer::Attachment>{{m_framebuffer_image.get(), RenderPass::LoadOp::Clear}};
		//auto depth = Framebuffer::Attachment(m_depth_image.get(), RenderPass::LoadOp::Clear);
		//m_target_framebuffer = Framebuffer(colors, depth);
		
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
		if (!m_active_scene->ecsRegistry()->isSceneLoaded())
		{
			return;
		}

		RenderGraph render_graph(m_resource_pool);

		BasicRenderer renderer = BasicRenderer::createRenderer(render_graph, m_active_scene->ecsRegistry());
		
		render_graph.execute(RenderModule::getBackend()->getCurrentCmdBuffer());
	

		//TODO this should be part of an editor pass
		std::vector<Descriptor> descriptor;
		ImageBase* target = render_graph.resourcePool()->getImage(renderer.m_editor_target);
		descriptor.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, target, SamplerType::RepeatLinear, VK_SHADER_STAGE_FRAGMENT_BIT);

		DescriptorSet desc_set{0, descriptor};
		m_texture_id = desc_set.descriptorSet();//ImGui_ImplVulkan_AddTexture(sampler->sampler(), final_image->imageView(), getImageLayout(final_image->imageUsage()));


	}




	void EngineViewportPanel::changeScene(EditorSceneChangedEvent& event)
	{
		m_active_scene = event.getNewScene();
		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());

	}
}
