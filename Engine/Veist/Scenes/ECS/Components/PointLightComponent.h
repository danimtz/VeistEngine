#pragma once

#include <glm/glm.hpp>

namespace Veist
{

struct PointLightComponent
{
	
	
	PointLightComponent(glm::vec3 colour = glm::vec3{ 1.0f }, float intensity = 1.0, float radius = 5.0) :
		m_intensity(intensity), m_colour(colour), m_radius(radius) {};
	PointLightComponent(const PointLightComponent&) = default;

	

	
	glm::vec3& colour() { return m_colour; };
	const glm::vec3& colour() const { return m_colour; };

	float& intensity() { return m_intensity; };
	float intensity() const { return m_intensity; };

	float& radius() { return m_radius; };
	float radius() const { return m_radius; };

private:
	glm::vec3 m_colour;
	float m_intensity;
	float m_radius;
	//no padding?
};

}
