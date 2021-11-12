#pragma once


#include "Engine/Renderer/Vulkan/Images/Image.h"

class LightProbe
{

public:

	LightProbe(const Cubemap& HDRcubemap, uint32_t irradiance_size = 64, uint32_t environment_size = 256, uint32_t brdf_size = 512);
	Cubemap* irradianceMap() const { return m_irradiance.get(); };
	Cubemap* prefilterMap() const { return m_environment_map.get(); };
	Texture* brdfLUT() const { return m_brdf_LUT.get(); };

private:


	std::unique_ptr<Cubemap> m_irradiance;
	std::unique_ptr<Cubemap> m_environment_map;
	std::unique_ptr<Texture> m_brdf_LUT;

	uint32_t m_irr_map_size;
	uint32_t m_env_map_size;
	uint32_t m_brdf_LUT_size;
};

