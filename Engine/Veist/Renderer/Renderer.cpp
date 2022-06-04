#include "pch.h"
#include "Renderer.h"

namespace Veist
{
	

	static float lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}


	SSAOsettings::SSAOsettings(uint32_t sample_count, uint32_t noise_texture_size) : m_sample_count(sample_count), m_noise_texture_size(noise_texture_size)
	{

		generateSamplesAndNoise();

	}



	void SSAOsettings::generateSamplesAndNoise()
	{
		m_ssao_samples.reserve(m_sample_count);

		std::uniform_real_distribution<float> random_float(0.0, 1.0);
		std::default_random_engine rng;

		for (uint32_t i = 0; i < m_sample_count; i++)
		{

			glm::vec3 ssao_sample = glm::normalize(glm::vec3(random_float(rng) * 2.0 - 1.0, random_float(rng) * 2.0 - 1.0, random_float(rng)));
			ssao_sample *= random_float(rng);
			
			float scale = (float)i / 64.0f;
			ssao_sample *= lerp(0.1f, 1.0f, scale * scale);

			m_ssao_samples.emplace_back(ssao_sample);
		}


		std::vector<glm::vec3> ssao_noise;
		for (uint32_t i = 0; i < (m_noise_texture_size* m_noise_texture_size); i++)
		{
			glm::vec3 noise( random_float(rng) * 2.0 - 1.0, random_float(rng) * 2.0 - 1.0, 0.0f);
			ssao_noise.push_back(noise);
		}


		ImageProperties texture_props(ImageSize(m_noise_texture_size, m_noise_texture_size), ImageFormat(VK_FORMAT_R16G16B16A16_SFLOAT));

		m_noise_texture = std::make_shared<Texture>(ssao_noise.data(), texture_props);

	}

	

}