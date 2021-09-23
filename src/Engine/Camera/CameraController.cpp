
#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

void CameraController::updateCamera(Camera& camera) 
{

	const float speed = m_cam_speed; //Delta time here?
	
	if (InputModule::isKeyPressed(GLFW_KEY_R)) //reset camera
	{
		camera.setPosition(glm::vec3{ 0.0f, 0.0f, -3.5f });
		camera.setViewMatrix(glm::lookAt(glm::vec3{ 0.0f, 0.0f, -3.5f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
		camera.setFoV(55);
	}


	if (InputModule::isKeyPressed(GLFW_KEY_W)) 
	{
		camera.setPosition( camera.position() + (speed * camera.front()));
	}

	if (InputModule::isKeyPressed(GLFW_KEY_S))
	{
		camera.setPosition(camera.position() - (speed * camera.front()));
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
		camera.setPosition(camera.position() + (speed * camera.up()));
	}

	if (InputModule::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		camera.setPosition(camera.position() - (speed * camera.up()));
	}


	if (InputModule::isMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
	{
		float deltaX = InputModule::getMouseDeltaX() * m_sensitivity;
		float deltaY = InputModule::getMouseDeltaY() * m_sensitivity;
		glm::vec3 world_up = glm::vec3{ 0.0f, 1.0f, 0.0f };
		

		auto pitch_qt = glm::angleAxis(deltaY, camera.right());
		auto yaw_qt = glm::angleAxis(deltaX, world_up); //world up

		
		glm::vec3 cam_front = pitch_qt * yaw_qt  * camera.front();
		glm::vec3 cam_right = yaw_qt * camera.right();

		if (glm::length(glm::cross(cam_front, world_up)) > 0.01f) {//limit 90 and -90 degrees
			camera.setViewMatrix(glm::lookAt(camera.position(), camera.position() + cam_front, glm::cross(-cam_front, cam_right )));
		}

		
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