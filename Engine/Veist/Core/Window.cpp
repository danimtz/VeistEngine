#include "pch.h"

#include "Window.h"
#include "Veist/Events/WindowEvents.h"
#include "Veist/Events/MouseEvents.h"
#include "Veist/Events/KeyboardEvents.h"

namespace Veist
{

	

	Window::Window(uint32_t width, uint32_t height, std::string& name)
	{

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_window_handle = glfwCreateWindow(1280, 720, name.c_str(), nullptr, nullptr);
		glfwSetWindowSizeLimits(m_window_handle, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

		m_data.width = width;
		m_data.height = height;
		m_data.name = name;

		glfwSetWindowUserPointer(m_window_handle, &m_data);

		setGLFWCallbacks();
	}


	void Window::onUpdate()
	{
		glfwPollEvents();
	}


	Window::~Window()
	{
		glfwDestroyWindow(m_window_handle);
		glfwTerminate();
	}



	void Window::setGLFWCallbacks()
	{
		glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			data.width = width;
			data.height = height;
			
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		
		});



		glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
			WindowCloseEvent event;
			data.eventCallback(event);
		});
		


		glfwSetKeyCallback(m_window_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
			}
		
		
		});



		glfwSetCharCallback(m_window_handle, [](GLFWwindow* window, unsigned int keycode)
		{

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		
		});



		glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* window, int button, int action, int mods)
		{
		
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		
		
		
		});



		glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* window, double xpos, double ypos)
		{
		
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xpos, (float)ypos);
			data.eventCallback(event);
		
		});



		glfwSetScrollCallback(m_window_handle, [](GLFWwindow* window, double xoffset, double yoffset)
		{
		
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data.eventCallback(event);
		
		});

	}

   


}