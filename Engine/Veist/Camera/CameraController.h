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

	void updateAspectRatio(float width, float height);

	Camera* camera() {return m_camera;};

	float& cameraSpeed() {return m_cam_speed;};

private:
	
	void updateFoV(MouseScrolledEvent& event);


	Camera* m_camera;

	glm::vec3 m_position = {0,0,0};

	float m_sensitivity{0.003f};
	float m_cam_speed{3.0f};
	float m_fovy{60.0f};
};

}