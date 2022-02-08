#pragma once

#include "Veist/Input/InputModule.h"
#include "Veist/Camera/Camera.h"

namespace Veist
{

class CameraController
{
public:
	
	CameraController(Camera* camera, glm::vec3 position = {0.0f, 0.0f, 3.5f }) : m_camera(camera), m_position(position) {};

	void onUpdate(Timestep dt);

	void onEvent(Event& event);

private:
	
	void updateFoV(MouseScrolledEvent& event);


	Camera* m_camera;

	glm::vec3 m_position = {0,0,0};

	float m_sensitivity{0.003f};
	float m_cam_speed{3.0f};
};

}