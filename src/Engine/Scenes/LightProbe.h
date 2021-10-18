#pragma once


#include "Engine/Renderer/Vulkan/Images/Image.h"

class LightProbe
{

public:

	static std::unique_ptr<LightProbe> Create(Cubemap& HDRcubemap);


	LightProbe() = default;

private:


	std::unique_ptr<Cubemap> m_irradiance;
	std::unique_ptr<Cubemap> m_environment_map;
	std::unique_ptr<Texture> m_brdf_LUT;


};

