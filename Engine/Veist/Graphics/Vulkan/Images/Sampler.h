#pragma once
#include <vulkan/vulkan.h>

namespace Veist
{

enum class SamplerType 
{
	None = 0,
	RepeatLinear,
	RepeatPoint,

	ClampLinear,
	ClampPoint,

	Shadow
};


class Sampler
{
public:
	Sampler(SamplerType type);

	VkSampler sampler() const { return m_sampler; };

private:
	VkSampler m_sampler;
};


}