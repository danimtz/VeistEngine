#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

Camera::Camera(float aspect_ratio, float fov, float far_plane, float near_plane, glm::vec3 pos)
{

	setViewMatrix(glm::lookAt(pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f })); //up vector negative 1 for now
	setProjectionMatrix(glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane));

	

	updateViewProjection();


	m_fustrum = Fustrum();//TODO

}



void Camera::setViewMatrix(const glm::mat4 view_mat) 
{
	m_view = view_mat;
	m_dirty_flag = true;
}

void Camera::setProjectionMatrix(const glm::mat4 proj_mat)
{
	m_projection = proj_mat;
	//m_projection[1][1] *= -1; //vulkan flips the y axis
	m_dirty_flag = true;
}


void Camera::updateViewProjection() 
{
	m_view_projection = (m_projection * m_view);
}


void Camera::setPosition(const glm::vec3 new_pos) 
{
	setViewMatrix(glm::lookAt(new_pos, new_pos + forward(), up()));
}


glm::vec3 Camera::position() const
{
	return glm::vec3{};
}

glm::vec3 Camera::up() const
{
	//Second row of view mat
	glm::vec3 up = { m_view[0][1], m_view[1][1] , m_view[2][1] };
	return glm::normalize(up);
}

glm::vec3 Camera::forward() const
{
	//Third row of view mat
	glm::vec3 forward = { m_view[0][2], m_view[1][2] , m_view[2][2] };
	return glm::normalize(-forward);
}

glm::vec3 Camera::right() const
{
	//First row of view mat
	glm::vec3 right = { m_view[0][2], m_view[1][2] , m_view[2][2] };
	return glm::normalize(right);
}



void Camera::onUpdate()
{
	if(m_dirty_flag){
		updateViewProjection();
		m_dirty_flag = false;
	}
}