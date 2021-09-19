#pragma once

#include "Engine/Input/InputModule.h"
#include "Engine/Camera/Camera.h"

class CameraController
{
public:
	
	CameraController() = default;

	void updateCamera(Camera& camera);


};

