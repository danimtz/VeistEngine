#pragma once

#include "Engine/Input/InputModule.h"
#include "Engine/Camera/Camera.h"

class CameraController
{
public:
	
	CameraController() = default;

	void updateCamera(Camera& camera);

private:
	float m_sensitivity{0.003f};
	float m_cam_speed{0.003f};
};

