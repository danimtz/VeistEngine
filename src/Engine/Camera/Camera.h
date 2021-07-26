#pragma once


#include <glm/glm.hpp>
#include "Fustrum.h"


class Camera
{
public:
	
	Camera() = default;
	Camera(float fov = 90.0f, float far_plane = 1000.0f, float near_plane = 0.1f );

	const glm::mat4& viewMatrix() const { return m_view; };
	const glm::mat4& projectionMatrix() const { return m_projection; };
	const glm::mat4& fustrum() const { return m_fustrum; };

private:

	//position maybe?


	glm::mat4 m_view;
	glm::mat4 m_projection;

	Fustrum m_fustrum;

};

