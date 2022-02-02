#pragma once
#include <vulkan/vulkan.h>

namespace Veist
{

enum class SamplerType 
{
	RepeatLinear,
	RepeatPoint,

	ClampLinear,
	ClampPoint
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