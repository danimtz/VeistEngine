#include "pch.h"
#include "InputModule.h"

namespace Veist
{

GLFWwindow* InputModule::m_window{nullptr};
float InputModule::m_mouse_lastX{0.0f};
float InputModule::m_mouse_lastY{0.0f};
float InputModule::m_mouse_deltaX{0.0f};
float InputModule::m_mouse_deltaY{0.0f};
float InputModule::m_mouse_scroll{0.0f};

//std::unordered_map<int, int> InputModule::m_keystate_map;
//std::unordered_map<int, int> InputModule::m_mouse_map;



void InputModule::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_mouse_scroll += yoffset;
}


void InputModule::init(GLFWwindow* window) 
{
	m_window = window;
	

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	//glfw set callbacks
	glfwSetScrollCallback(m_window, InputModule::mouseScrollCallback);
}

static void calculateMouseDelta(GLFWwindow* m_window, float& lastX, float& lastY, float& deltaX, float& deltaY)
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	deltaX = float(lastX - xpos);
	deltaY = float(lastY - ypos);
	lastX = xpos;
	lastY = ypos;
}

void InputModule::onUpdate()
{

	m_mouse_scroll = 0;
	//calculate mouse movement delta
	calculateMouseDelta(m_window, m_mouse_lastX, m_mouse_lastY, m_mouse_deltaX, m_mouse_deltaY);


	if (InputModule::isKeyPressed(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	glfwPollEvents();
}


bool InputModule::isKeyPressed(int keycode) 
{

	auto state = glfwGetKey(m_window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;

}


bool InputModule::isMouseButtonPressed(int button) 
{

	auto state = glfwGetMouseButton(m_window, button);
	return state == GLFW_PRESS || state == GLFW_REPEAT;

}


std::pair<float, float> InputModule::getMousePos() 
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	
	return std::make_pair(xpos, ypos);
}


float InputModule::getMouseX() 
{
	auto [xpos, temp] = getMousePos();
	return xpos;
}


float InputModule::getMouseY() 
{
	auto [temp, ypos] = getMousePos();
	return ypos;
}



}