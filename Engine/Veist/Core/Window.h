#pragma once

#include <GLFW/glfw3.h>
#include "Veist/Events/Event.h"


namespace Veist
{

	class Window
	{
	public:

		Window(uint32_t width, uint32_t height, std::string& name);
		~Window();

		void onUpdate();

		GLFWwindow* windowHandle() const { return m_window_handle; };

		void setEventCallback(const std::function<void(Event&)>& callback) { m_data.eventCallback = callback;};
		
	private:
		
		void setGLFWCallbacks();

		GLFWwindow* m_window_handle;
		
		struct WindowData
		{
			std::string name;
			uint32_t height, width;

			std::function<void(Event&)> eventCallback;
		};

		WindowData m_data;
	


	};


}