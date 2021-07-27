#pragma once


#include <glm/glm.hpp>
#include "Fustrum.h"


class Camera
{
public:
	
	//Camera() = default;
	Camera(float aspect_ratio = 1280.0f/720.0f, float fov = 70.0f, float far_plane = 1000.0f, float near_plane = 0.1f, glm::vec3 pos = {0.0f, 0.0f, -2.0f} );

	const glm::mat4& viewMatrix() const { return m_view; };
	const glm::mat4& projectionMatrix() const { return m_projection; };
	const glm::mat4& viewProjectionMatrix() const { return m_view_projection; };
	const Fustrum& fustrum() const { return m_fustrum; };

	glm::vec3 position() const;//TODO
	glm::vec3 up() const;
	glm::vec3 forward() const;
	glm::vec3 right() const;


	void setPosition(const glm::vec3 new_pos);
	void setViewMatrix(const glm::mat4 view_mat);
	void setProjectionMatrix(const glm::mat4 proj_mat);
	void setViewProjectionMatrix(const glm::mat4 view_proj_mat);


private:

	void updateViewProjection();

	//position maybe?

	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_view_projection;

	Fustrum m_fustrum;

};

