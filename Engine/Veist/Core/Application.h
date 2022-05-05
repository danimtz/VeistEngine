#pragma once



#include "Veist/Graphics/RenderModule.h"
#include "Veist/ImGUI/GUIModule.h"
#include "Veist/Input/InputModule.h"
#include "Veist/Core/Window.h"
#include "Veist/Scenes/Scene.h"
#include "Veist/Concurrency/ThreadPool.h"
#include "Veist/Events/WindowEvents.h"

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
		virtual void onEvent(Event& event) = 0;


		void processEvent(Event& event);

		void onWindowClose(WindowCloseEvent& event);
		void onWindowResize(WindowResizeEvent& event);

		void close() { m_running = false; };

		

	protected:

		
		


	protected:


		bool m_running = true;
		bool m_minimized = false;
		Window* m_window;
		ThreadPool* m_thread_pool;
		float m_last_frame_time = 0.0f;



	};

	Application* CreateApplication();

}