#pragma once

#include <glm/glm.hpp>

struct DirectionalLightComponent
{

	DirectionalLightComponent(glm::vec3 direction = glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3 colour = glm::vec3{ 1.0f }, float intensity = 1.0) :
		m_direction(direction), m_colour(colour), m_intensity(intensity)
	{};

	DirectionalLightComponent(const DirectionalLightComponent&) = default;


	
	glm::vec3& direction() { return m_direction; };
	const glm::vec3& direction() const { return m_direction; };

	glm::vec3& colour() { return m_colour; };
	const glm::vec3& colour() const { return m_colour; };

	float& intensity() { return m_intensity; };
	float intensity() const { return m_intensity; };

private:
	glm::vec3 m_direction;
	float m_intensity;
	glm::vec3 m_colour;
	//no padding?
};