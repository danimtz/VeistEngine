#include "pch.h"
#include "Sampler.h"
#include "Veist/Graphics/RenderModule.h"


namespace Veist
{

static VkSamplerAddressMode getAddressMode(SamplerType type) {
	switch (type) {
		case SamplerType::ClampLinear:
		case SamplerType::ClampPoint:
		case SamplerType::Shadow:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		case SamplerType::RepeatLinear:
		case SamplerType::RepeatPoint:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			
		default:
			CRITICAL_ERROR_LOG("Invalid sampler address type");
	}

}

static VkFilter getFilter(SamplerType type) {
	switch (type) {
	case SamplerType::ClampLinear:
	case SamplerType::RepeatLinear:
	case SamplerType::Shadow:
		return VK_FILTER_LINEAR;

	case SamplerType::RepeatPoint:
	case SamplerType::ClampPoint:
		return VK_FILTER_NEAREST;

	default:
		CRITICAL_ERROR_LOG("Invalid sampler filter type");
	}
}

static VkSamplerMipmapMode getMipmapMode(SamplerType type) {
	switch (type) {
	case SamplerType::ClampLinear:
	case SamplerType::RepeatLinear:
	case SamplerType::Shadow:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;

	case SamplerType::ClampPoint:
	case SamplerType::RepeatPoint:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	default:
		CRITICAL_ERROR_LOG("Invalid sampler mipmap type");
	}
}

static VkSamplerCreateInfo createSamplerInfo(SamplerType type) 
{
	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext = nullptr;

	auto address_mode = getAddressMode(type);
	auto filter = getFilter(type);
	auto mipmap_mode = getMipmapMode(type);

	info.addressModeU = address_mode;
	info.addressModeV = address_mode;
	info.addressModeW = address_mode;
	info.magFilter = filter;
	info.minFilter = filter;
	info.mipmapMode = mipmap_mode;
	info.maxLod = 1000.0f;

	auto gpuinfo = RenderModule::getBackend()->getGPUinfo();

	info.maxAnisotropy = gpuinfo.properties.limits.maxSamplerAnisotropy;
	info.anisotropyEnable = VK_FALSE;

	info.compareEnable = (type == SamplerType::Shadow);
	info.compareOp = VK_COMPARE_OP_LESS;

	return info;
}

Sampler::Sampler(SamplerType type) {
	VkDevice device = RenderModule::getBackend()->getDevice();
	
	VkSamplerCreateInfo sampler_info = createSamplerInfo(type);
	
	VkSampler sampler;
	VK_CHECK(vkCreateSampler(device, &sampler_info, nullptr, &sampler));

	m_sampler = sampler;

	RenderModule::getBackend()->pushToDeletionQueue([=]{vkDestroySampler(device, sampler, nullptr);});
}

}