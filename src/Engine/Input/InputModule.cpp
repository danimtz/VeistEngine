#include "InputModule.h"


GLFWwindow* InputModule::m_window{nullptr};


bool InputModule::isKeyPressed(int keycode) {

	auto state = glfwGetKey(m_window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;

}


bool InputModule::isMouseButtonPressed(int button) {

	auto state = glfwGetMouseButton(m_window, button);
	return state == GLFW_PRESS || state == GLFW_REPEAT;

}


float InputModule::getMouseX() {

	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	return xpos;
}


float InputModule::getMouseY() {

	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	return ypos;
}