#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>



struct TransformComponent
{

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(glm::vec3& translation) : m_translation(translation) {}
	TransformComponent(glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) : m_translation(translation), m_rotation(rotation), m_scale(scale) {}
	
	glm::vec3& scale() {return m_scale;};
	const glm::vec3& scale() const { return m_scale; };

	glm::vec3& translation() { return m_translation; };
	const glm::vec3& translation() const { return m_translation; };

	glm::vec3& rotation() { return m_rotation; };
	const glm::vec3& rotation() const { return m_rotation; };


	glm::mat4 getTransform() const
	{
		glm::mat4 rotation = glm::mat4(glm::quat(m_rotation));
		return glm::translate(glm::mat4(1.0f), m_translation) * rotation * glm::scale(glm::mat4(1.0f), m_scale);
	}



private:
	glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 m_translation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_rotation{ 0.0f, 0.0f, 0.0f };
};


