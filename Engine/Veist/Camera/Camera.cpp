#include "pch.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>


namespace Veist
{



Camera::Camera(float aspect_ratio, float fov, float far_plane, float near_plane, glm::vec3 pos) : m_fovy(fov), m_clip_planes({near_plane, far_plane})
{

	setViewMatrix(glm::lookAt(pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f })); //up vector negative 1 for now vulkan coordinate system
	setProjectionMatrix(glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane));

	

	updateViewProjection();


	m_fustrum = Fustrum();//TODO

}


void Camera::setFoVy(float new_fov)
{
	float oneOverAR = m_projection[0][0]/m_projection[1][1];
	float half_fov = glm::radians(new_fov)/2.0f;

	float new_F = cos(half_fov)/sin(half_fov);

	m_projection[0][0] = oneOverAR * -new_F;
	m_projection[1][1] = -new_F;

	m_dirty_flag = true;
	m_fovy = new_fov;
}

void Camera::setViewMatrix(const glm::mat4 view_mat) 
{
	m_view = view_mat;
	m_dirty_flag = true;
}

void Camera::setProjectionMatrix(const glm::mat4 proj_mat)
{
	m_projection = proj_mat;
	m_projection[1][1] *= -1; //vulkan flips the y axis
	m_dirty_flag = true;
}


void Camera::updateViewProjection() 
{
	m_view_projection = (m_projection * m_view);
}


void Camera::setPosition(const glm::vec3 new_pos) 
{
	setViewMatrix(glm::lookAt(new_pos, new_pos + front(), up()));
}



glm::vec3 Camera::up() const
{
	//Second row of view mat
	glm::vec3 up = { m_view[0][1], m_view[1][1] , m_view[2][1] };
	return glm::normalize(up);
}

glm::vec3 Camera::front() const
{
	//Third row of view mat
	glm::vec3 forward = { m_view[0][2], m_view[1][2] , m_view[2][2] };
	return glm::normalize(-forward);
}

glm::vec3 Camera::right() const
{
	//First row of view mat
	glm::vec3 right = { m_view[0][0], m_view[1][0] , m_view[2][0] };
	return glm::normalize(right);
}



void Camera::onUpdate()
{
	if(m_dirty_flag){
		updateViewProjection();
		m_dirty_flag = false;
	}
}

}