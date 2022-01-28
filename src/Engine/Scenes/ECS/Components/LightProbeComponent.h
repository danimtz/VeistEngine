#pragma once

#include "Engine/Scenes/LightProbe.h"

struct LightProbeComponent
{

	LightProbeComponent() = default;
	LightProbeComponent(const LightProbeComponent&) = default;

	//TEMPORARY, TODO: remove this function (when asset system is implemented etc)
	LightProbeComponent(LightProbe& probe) : m_irradiance(probe.irradianceMap()), m_environment_map(probe.prefilterMap()), m_brdf_LUT(probe.brdfLUT()) {};


	Cubemap* irradianceMap() const { return m_irradiance.get(); };
	Cubemap* prefilterMap() const { return m_environment_map.get(); };
	Texture* brdfLUT() const { return m_brdf_LUT.get(); };

private:

	std::shared_ptr<Cubemap> m_irradiance;
	std::shared_ptr<Cubemap> m_environment_map;
	std::shared_ptr<Texture> m_brdf_LUT;


};