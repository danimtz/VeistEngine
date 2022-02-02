#pragma once


#include "Veist/Camera/Camera.h"



namespace Veist
{

struct CameraComponent
{

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(Camera camera) : m_camera(camera) {};


	Camera& camera() { return m_camera; };
	const Camera& camera() const { return m_camera; };

private:

	Camera m_camera;

};


}
