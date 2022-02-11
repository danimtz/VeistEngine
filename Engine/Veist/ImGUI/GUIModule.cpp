#include "pch.h"
#include "GUIModule.h"

#include "Veist/Renderer/Vulkan/RenderBackend.h"


namespace Veist
{


	void GUIModule::init(RenderBackend* render_backend) 
	{

		//Init context
		ImGui::CreateContext();

		//Config
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.Fonts->AddFontFromFileTTF("..\\assets\\fonts\\SourceSansPro\\SourceSansPro-Light.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("..\\assets\\fonts\\SourceSansPro\\SourceSansPro-Bold.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("..\\assets\\fonts\\SourceSansPro\\SourceSansPro-Regular.ttf", 18.0f);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("..\\assets\\fonts\\SourceSansPro\\SourceSansPro-Regular.ttf", 18.0f);
	

		//Initialize imgui with vulkan/api
		render_backend->initImGUI();

		//IF IMGUI FAILS WHEN REBUILDING PROJECT I CHANGED LINE 1486 in impl_vulkan.cpp to allow a pipeline to be created for the new viewport and not use the default renderpasses avoiding validation layer errors
		//ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline, 0 /*bd->Subpass*/); 

		// Setup style
		ImGui::StyleColorsDark();




		//Edit style
		setImGuiStyle();

	}



	void GUIModule::beginFrame()
	{
		//A bit too vulkan specific. This should change depending on platform

		//End previous renderpass


		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


	}


	void GUIModule::endFrame()
	{

		ImGuiIO& io = ImGui::GetIO();

		RenderBackend* backend = RenderModule::getBackend().get();
	
		//GUI renderpass
		CommandBuffer& cmd_buffer = backend->getCurrentCmdBuffer();
		cmd_buffer.beginRenderPass(backend->getCurrentFramebuffer());

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), backend->getCurrentCmdBuffer().vk_commandBuffer());

		cmd_buffer.endRenderPass();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

    

	
	}


	void GUIModule::onUpdate()
	{

		GUIModule::beginFrame();
		{
			//module or whatever->onUpdateImGUI()
			ImGui::ShowDemoWindow();
		}
		GUIModule::endFrame();

	}



	void GUIModule::shutdown()
	{

	   //ImGui::DestroyContext();

	}




	void GUIModule::setImGuiStyle()
	{

	
		auto& colors = ImGui::GetStyle().Colors;
		auto& style = ImGui::GetStyle();
		//colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };


		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.FrameRounding = 2.0f;
		
	}

}