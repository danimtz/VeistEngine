#pragma once


#include <glm/glm.hpp>
#include <array>

class Fustrum
{
public:

	Fustrum() = default;


private:

	std::array<glm::vec4, 6> m_normals;

};
