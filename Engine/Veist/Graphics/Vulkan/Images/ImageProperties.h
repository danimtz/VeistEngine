#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Veist/Graphics/Vulkan/Images/ImageFormat.h"





namespace Veist
{



struct ImageSize {
	ImageSize() = default;
	ImageSize(glm::vec2& size) : width(size.x), height(size.y), depth(1), n_channels(4) {};
	ImageSize(uint32_t w, uint32_t h) : width(w), height(h), depth(1), n_channels(4) {};
	ImageSize(uint32_t w, uint32_t h, uint32_t n_c) : width(w), height(h), depth(1), n_channels(n_c) {};
	ImageSize(uint32_t w, uint32_t h, uint32_t d, uint32_t n_c) : width(w), height(h), depth(d), n_channels(n_c) {};
	uint32_t width{0};
	uint32_t height{0};
	uint32_t depth{0};
	uint32_t n_channels{0};
};


class ImageProperties
{
public:

	ImageProperties() = default;
	
	ImageProperties(ImageSize size, ImageFormat format, uint32_t mip_levels = 1, uint32_t layers = 1) : m_size(size), m_format(format), m_mip_levels(mip_levels), m_layers(layers) {};


	const ImageSize& imageSize() const { return m_size; };
	const ImageFormat& imageFormat() const { return m_format; };
	uint32_t mipLevels() const { return m_mip_levels; };
	uint32_t layerCount() const { return m_layers; };
	
	size_t bufferOffset(uint32_t layer = 0, uint32_t mip_level = 0);

	uint32_t sizeInPixels();
	uint32_t layerSizeBytes();

private:

	ImageSize m_size;
	ImageFormat m_format;
	uint32_t m_mip_levels = 1;
	uint32_t m_layers = 1;


	uint32_t mipLevelBytes(uint32_t mip_level);
	glm::u32vec3 mipSize(uint32_t mip_level);

};


}