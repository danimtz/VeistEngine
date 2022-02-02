#pragma once

#include "Veist/Input/InputModule.h"
#include "Veist/Camera/Camera.h"
namespace Veist
{

class CameraController
{
public:
	
	CameraController() = default;

	void updateCamera(Camera& camera, glm::vec3& position, Timestep dt);

private:
	float m_sensitivity{0.003f};
	float m_cam_speed{3.0f};
};

}