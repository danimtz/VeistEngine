#include "pch.h"

#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Veist
{



	void CameraController::onUpdate(Timestep dt) 
	{

		const float speed = m_cam_speed; //Delta time here?
	
		if (InputModule::isKeyPressed(GLFW_KEY_R)) //reset camera
		{
			m_position = glm::vec3{ 0.0f, 0.0f, 3.5f };
			m_camera->setPosition(m_position);
			m_camera->setViewMatrix(glm::lookAt(glm::vec3{ 0.0f, 0.0f, -3.5f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
			m_camera->setFoV(55);
		}


		if (InputModule::isKeyPressed(GLFW_KEY_W)) 
		{
			m_position = m_position + (speed * m_camera->front() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}

		if (InputModule::isKeyPressed(GLFW_KEY_S))
		{
			m_position = m_position - (speed * m_camera->front() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}


		if (InputModule::isKeyPressed(GLFW_KEY_A))
		{
			m_position = m_position - (speed * m_camera->right() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}

		if (InputModule::isKeyPressed(GLFW_KEY_D))
		{
			m_position = m_position + (speed * m_camera->right() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}


		if (InputModule::isKeyPressed(GLFW_KEY_SPACE))
		{	
			m_position = m_position + (speed * m_camera->up() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}

		if (InputModule::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
		{
			m_position = m_position - (speed * m_camera->up() * dt.getSeconds());
			m_camera->setPosition(m_position);
		}


		if (InputModule::isMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
		{
			float deltaX = InputModule::getMouseDeltaX() * m_sensitivity;
			float deltaY = InputModule::getMouseDeltaY() * m_sensitivity;
			glm::vec3 world_up = glm::vec3{ 0.0f, 1.0f, 0.0f };
		

			auto pitch_qt = glm::angleAxis(deltaY, m_camera->right());
			auto yaw_qt = glm::angleAxis(deltaX, world_up); //world up

		
			glm::vec3 cam_front = pitch_qt * yaw_qt  * m_camera->front();
			glm::vec3 cam_right = yaw_qt * m_camera->right();

			if (glm::length(glm::cross(cam_front, world_up)) > 0.01f) {//limit 90 and -90 degrees
				m_camera->setViewMatrix(glm::lookAt(m_position, m_position + cam_front, glm::cross(-cam_front, cam_right )));
			}

		
		}


	}

	void CameraController::onEvent(Event& event)
	{
		EventHandler handler(event);

		handler.handleEvent<MouseScrolledEvent>(VEIST_EVENT_BIND_FUNCTION(CameraController::updateFoV));

	}
	
	void CameraController::updateFoV(MouseScrolledEvent& event)
	{

		m_camera->setFoV(m_camera->fov() - event.getOffsetY());

		if (m_camera->fov() > 105.0f)
		{
			m_camera->setFoV(105.5);
		}
		if (m_camera->fov() < 30.0f)
		{
			m_camera->setFoV(30.0f);
		}
		
	
	}


}