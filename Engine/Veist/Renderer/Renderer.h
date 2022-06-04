#pragma once

#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"

namespace Veist
{

//Not implemented yet


	struct SSAOsettings
	{
		SSAOsettings(uint32_t sample_count = 64, uint32_t noise_texture_size = 4);

		std::vector<glm::vec3> m_ssao_samples;
		std::shared_ptr<Texture> m_noise_texture;
		uint32_t m_sample_count;
		uint32_t m_noise_texture_size;
		
	private:
		

			
		void generateSamplesAndNoise();
		
	};



	struct RendererSettings
	{
		
		RendererSettings() = default;

		uint32_t m_shadow_map_size = 2048;
		uint32_t m_ortho_bounds = 20;

		SSAOsettings m_ssao_settings;

	};




	struct Renderer 
	{

		Renderer() = default;
		virtual ~Renderer(){};


		glm::vec2 m_size = {};
		RendererSettings* m_settings;
		RenderGraph::ImageResource* m_renderer_target{ nullptr };


	};


}