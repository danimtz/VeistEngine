#include <pch.h>
#include "EngineViewportPanel.h"



namespace VeistEditor
{
	
	EngineViewportPanel::EngineViewportPanel()
	{
		//TODO: image props should be the size of the imgui window
		ImageProperties img_props = ImageProperties({ m_viewport_size }, {VK_FORMAT_R8G8B8A8_SRGB});
		ImageProperties depth_img_props = ImageProperties({ m_viewport_size }, { VK_FORMAT_D32_SFLOAT });

		//Create images
		/*for (int i = 0; i < RenderModule::getBackend()->getSwapchainImageCount(); i++)
		{
			m_framebuffer_images.emplace_back( std::make_unique<ColorTextureAttachment>(img_props) );
		}*/
		m_framebuffer_image = std::make_unique<ColorTextureAttachment>(img_props);
		m_depth_image = std::make_unique<DepthTextureAttachment>(depth_img_props);


		//Create framebuffer (only using 1 image for now)
		m_target_framebuffer = { m_framebuffer_image.get(), m_depth_image.get(), Framebuffer::LoadOp::Clear };


		m_texture_id = ImGui_ImplVulkan_AddTexture(Sampler(SamplerType::RepeatLinear).sampler(), m_framebuffer_image.get()->imageView(), getImageLayout(m_framebuffer_image.get()->imageUsage()));
	}

	EngineViewportPanel::~EngineViewportPanel()
	{

	}


	void EngineViewportPanel::renderPanel()
	{
		ImGui::Begin("Viewport", nullptr);

		

		
		CommandBuffer& cmd_buffer = RenderModule::getBackend()->getCurrentCmdBuffer();

		cmd_buffer.beginRenderPass(m_target_framebuffer);

		RenderModule::renderScene(cmd_buffer);

		cmd_buffer.endRenderPass();

	

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		viewport_panel_size.x = viewport_panel_size.y * m_aspect_ratio;

		ImGui::SetNextWindowSizeConstraints(viewport_panel_size, viewport_panel_size);

		ImGui::Image(m_texture_id, viewport_panel_size);

		ImGui::End();

	}


}
