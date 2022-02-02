#pragma once

#include <vulkan/vulkan.h>

namespace Veist
{


class ImageFormat {
public:
	ImageFormat(VkFormat format = VK_FORMAT_UNDEFINED) : m_format(format) {};

	VkImageAspectFlags imageAspectFlags() const;
	
	const VkFormat format() const { return m_format; };
	uint32_t bytesPerPixel() const;

private:
	VkFormat m_format;
};

}