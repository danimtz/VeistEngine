
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


	if (InputModule::isMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
	{
		
		float deltaX = InputModule::getMouseDeltaX();
		float deltaY = InputModule::getMouseDeltaY();
		
		camera.setPosition(camera.position() - 0.01f * deltaX * camera.right());
		camera.setPosition(camera.position() - 0.01f * deltaY * camera.up());
	}


	if (InputModule::getMouseScroll() != 0) 
	{
		camera.setFoV(camera.fov() - InputModule::getMouseScroll());
		
		if (camera.fov() > 105.0f)
		{
			camera.setFoV(105.5);
		}
		if (camera.fov() < 30.0f)
		{
			camera.setFoV(30.0f);
		}
	}
}