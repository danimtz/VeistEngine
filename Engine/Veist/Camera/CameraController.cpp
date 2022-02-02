#include "pch.h"

#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Veist
{



void CameraController::updateCamera(Camera& camera, glm::vec3& position, Timestep dt) 
{

	const float speed = m_cam_speed; //Delta time here?
	
	if (InputModule::isKeyPressed(GLFW_KEY_R)) //reset camera
	{
		position = glm::vec3{ 0.0f, 0.0f, -3.5f };
		camera.setPosition(position);
		camera.setViewMatrix(glm::lookAt(glm::vec3{ 0.0f, 0.0f, -3.5f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
		camera.setFoV(55);
	}


	if (InputModule::isKeyPressed(GLFW_KEY_W)) 
	{
		position = position + (speed * camera.front() * dt.getSeconds());
		camera.setPosition(position);
	}

	if (InputModule::isKeyPressed(GLFW_KEY_S))
	{
		position = position - (speed * camera.front() * dt.getSeconds());
		camera.setPosition(position);
	}


	if (InputModule::isKeyPressed(GLFW_KEY_A))
	{
		position = position - (speed * camera.right() * dt.getSeconds());
		camera.setPosition(position);
	}

	if (InputModule::isKeyPressed(GLFW_KEY_D))
	{
		position = position + (speed * camera.right() * dt.getSeconds());
		camera.setPosition(position);
	}


	if (InputModule::isKeyPressed(GLFW_KEY_SPACE))
	{	
		position = position + (speed * camera.up() * dt.getSeconds());
		camera.setPosition(position);
	}

	if (InputModule::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		position = position - (speed * camera.up() * dt.getSeconds());
		camera.setPosition(position);
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
			camera.setViewMatrix(glm::lookAt(position, position + cam_front, glm::cross(-cam_front, cam_right )));
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

}