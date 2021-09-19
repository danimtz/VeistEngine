
#include "CameraController.h"


void CameraController::updateCamera(Camera& camera) 
{

	const float speed = 0.01f; //Delta time here?
	
	if (InputModule::isKeyPressed(GLFW_KEY_W)) 
	{
		camera.setPosition( camera.position() + (speed * camera.forward()));
	}

	if (InputModule::isKeyPressed(GLFW_KEY_S))
	{
		camera.setPosition(camera.position() - (speed * camera.forward()));
	}


	if (InputModule::isKeyPressed(GLFW_KEY_A))
	{
		camera.setPosition(camera.position() - (speed * camera.right()));
	}

	if (InputModule::isKeyPressed(GLFW_KEY_D))
	{
		camera.setPosition(camera.position() + (speed * camera.right()));
	}


	if (InputModule::isKeyPressed(GLFW_KEY_SPACE))
	{
		camera.setPosition(camera.position() - (speed * camera.up()));
	}

	if (InputModule::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		camera.setPosition(camera.position() + (speed * camera.up()));
	}


	if (InputModule::isMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
	{
		

	}
}