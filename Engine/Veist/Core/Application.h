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
		virtual ~Application();

		//Declare in client
		virtual void runClient() = 0;
		virtual void initClient() = 0;
		virtual void shutdownClient() = 0;

		static Application& get() {return *s_Instance;};

	protected:

		
		void shutdown();



	protected:
		bool m_running = true;
		GLFWwindow* m_window;
		float m_last_frame_time = 0.0f;

	private:

		static Application* s_Instance;

	};

	Application* CreateApplication();

}