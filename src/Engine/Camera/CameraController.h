#pragma once

#include "Engine/Input/InputModule.h"
#include "Engine/Camera/Camera.h"

class CameraController
{
public:
	
	CameraController() = default;

	void updateCamera(Camera& camera, glm::vec3& position, Timestep dt);

private:
	float m_sensitivity{0.003f};
	float m_cam_speed{3.0f};
};

