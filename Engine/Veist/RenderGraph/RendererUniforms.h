#pragma once


#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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
			glm::mat4 inverse_projection;
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

		struct ObjectMatrices
		{
			glm::mat4 model = {};
			glm::mat4 normal = {};
		};

		struct ShadowMapData
		{
			glm::mat4 lightspace = {};
			uint32_t map_size = {};
		};


		struct SceneInfo
		{
			float dir_lights_count;
			float point_lights_count;
			//more to be added
		};

	}

}