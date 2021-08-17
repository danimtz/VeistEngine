#pragma once



#include <glm/glm.hpp>

class PointLight
{
public:
	PointLight(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3 colour = glm::vec3{ 1.0f }, float intensity = 1.0, float radius = 5.0) :
		m_position(position), m_intensity(intensity), m_colour(colour), m_radius(radius) {};

	glm::vec3& position() { return m_position; };
	const glm::vec3& position() const { return m_position; };

	glm::vec3& colour() { return m_colour; };
	const glm::vec3& colour() const { return m_colour; };

	float& intensity() { return m_intensity; };
	float intensity() const { return m_intensity; };

private:
	glm::vec3 m_position;
	float m_intensity;
	glm::vec3 m_colour;
	float m_radius;

	//attenuation? falloff disatnce? quadratic linear etc?
};

