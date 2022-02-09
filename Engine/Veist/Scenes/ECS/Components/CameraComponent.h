#pragma once


#include "Veist/Camera/Camera.h"



namespace Veist
{

	struct CameraComponent
	{

		CameraComponent() : m_camera(std::make_shared<Camera>()) {};
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(std::shared_ptr<Camera> camera_ptr) : m_camera(camera_ptr) {}; //not owning. used for main camera. scene class own unique pointer


		Camera* camera() { return m_camera.get(); };
		const Camera* camera() const { return m_camera.get(); };

	private:

		std::shared_ptr<Camera> m_camera;

	};


}
