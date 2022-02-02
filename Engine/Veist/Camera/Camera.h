#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Fustrum.h"


namespace Veist
{


class Camera
{
public:
	
	//Camera() = default;
	Camera(float aspect_ratio = 1280.0f/720.0f, float fov = 55.0f, float far_plane = 1000.0f, float near_plane = 0.1f, glm::vec3 pos = {0.0f, 0.0f, 3.5f} );

	const glm::mat4& viewMatrix() const { return m_view; };
	const glm::mat4& projectionMatrix() const { return m_projection; };
	const glm::mat4& viewProjectionMatrix() const { return m_view_projection; };
	const Fustrum& fustrum() const { return m_fustrum; };
	const float& fov() const { return m_fov; };


	
	glm::vec3 up() const;
	glm::vec3 front() const;
	glm::vec3 right() const;


	void setPosition(const glm::vec3 new_pos);
	void setFoV(float new_fov);
	void setViewMatrix(const glm::mat4 view_mat);
	void setProjectionMatrix(const glm::mat4 proj_mat);
	


	void onUpdate();

private:

	void updateViewProjection();
	//void setViewProjectionMatrix(const glm::mat4 view_proj_mat);

	//position maybe?

	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_view_projection;


	float m_fov;

	Fustrum m_fustrum;

	mutable bool m_dirty_flag = {false};

};

}