#pragma once


#include <GLFW/glfw3.h>

class InputModule
{
public:
	
	static void init(GLFWwindow* window) { m_window = window; };
	static void shutdown() { m_window = nullptr; };

	static bool isKeyPressed(int keycode);
	static bool isMouseButtonPressed(int keycode);
	
private:
	
	static GLFWwindow* m_window;

};

