#pragma once


#include <GLFW/glfw3.h>
#include "KeyCodes.h"

class InputModule
{
public:
	
	static void init(GLFWwindow* window);
	static void shutdown() { m_window = nullptr; };
	static void onUpdate();

	static bool isKeyPressed(int keycode);
	static bool isMouseButtonPressed(int keycode);
	
	static float getMouseX();
	static float getMouseY();

	static std::pair<float, float> getMousePos();
	static float getMouseDeltaX() { return m_mouse_deltaX; };//mouse delta has moved since last frame
	static float getMouseDeltaY() { return m_mouse_deltaY; };

	static float getMouseScroll() { return m_mouse_scroll; };

private:
	
	static GLFWwindow* m_window;
	static float m_mouse_lastX;
	static float m_mouse_lastY;
	static float m_mouse_deltaX;
	static float m_mouse_deltaY;
	static float m_mouse_scroll;

	//static std::unordered_map<int, int> m_keystate_map;
	//static std::unordered_map<int, int> m_mouse_map;

	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

