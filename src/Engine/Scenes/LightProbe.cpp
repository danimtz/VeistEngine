
#include "LightProbe.h"

#include "Engine/Renderer/RenderModule.h"






std::unique_ptr<LightProbe> LightProbe::Create(Cubemap& HDRcubemap)
{
	//TODO: create light probe
	// irradiance map convolution
	// environment map roughness mipmaps
	// BRDF LUT

	return std::make_unique<LightProbe>(LightProbe());
}