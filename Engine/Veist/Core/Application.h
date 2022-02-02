#pragma once



#include "Veist/Renderer/RenderModule.h"
#include "Veist/ImGUI/GUIModule.h"
#include "Veist/Input/InputModule.h"
#include "Veist/Scenes/Scene.h"


namespace Veist
{

	class Application
	{
	public:

		Application(std::string name);

		void run();
	
		static Application& get() {return *s_Instance;};

	private:

		void shutdown();



	private:
		bool m_running = true;
		GLFWwindow* m_window;
		float m_last_frame_time = 0.0f;
	
		Scene* scene;//TEMPORARY replace with actual layer that runs the scene i.e editor or runtime


	private:

		static Application* s_Instance;

	};

	Application* CreateApplication();

}