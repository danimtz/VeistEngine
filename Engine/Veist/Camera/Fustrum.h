#pragma once


#include <glm/glm.hpp>


namespace Veist
{



	class Fustrum
	{
	public:

		Fustrum() = default;


	private:

		// 0:Near, 1:Far, 2:Left, 3:Right, 4:Top, 5:Down // Ax + By + Cz + D = 0 plane equation
		std::array<glm::vec4, 6> m_planes; 

	};



}