#pragma once


#include "Veist/Renderer/Vulkan/Images/Image.h"

namespace Veist
{

class LightProbe
{

public:

	LightProbe(const Cubemap& HDRcubemap, uint32_t irradiance_size = 64, uint32_t environment_size = 256, uint32_t brdf_size = 512);
	std::shared_ptr<Cubemap> irradianceMap() const { return m_irradiance; };
	std::shared_ptr<Cubemap> prefilterMap() const { return m_environment_map; };
	std::shared_ptr<Texture> brdfLUT() const { return m_brdf_LUT; };

private:


	std::shared_ptr<Cubemap> m_irradiance;
	std::shared_ptr<Cubemap> m_environment_map;
	std::shared_ptr<Texture> m_brdf_LUT;

	uint32_t m_irr_map_size;
	uint32_t m_env_map_size;
	uint32_t m_brdf_LUT_size;
};

}