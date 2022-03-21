#pragma once


#include <glm/glm.hpp>

namespace Veist
{

	namespace RendererUniforms
	{

		struct CameraData
		{
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 view_projection;
			glm::mat4 inverse_view;
		};

		struct GPUDirLight
		{
			glm::vec3 direction;
			float intensity;
			glm::vec3 colour;
			uint32_t padding;
		};

		struct GPUPointLight
		{
			glm::vec3 position;
			float intensity;
			glm::vec3 colour;
			float radius;
		};

		struct SceneInfo
		{
			float dir_lights_count;
			float point_lights_count;
			//more to be added
		};

	}

}